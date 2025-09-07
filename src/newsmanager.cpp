#include "newsmanager.h"

#include <string>
#include <vector>
#include <thread>
#include <curl/curl.h>
#include <chrono>
#include <random>
#include <iostream>
#include <fstream>


#include "vlistmodel.h"
#include <QDir>
#include <QtConcurrent/QtConcurrentRun>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QTranslator>
#include <QRegularExpression>
#include <QXmlStreamReader>

NewsManager::NewsManager(VListModel &model, VListModel &channelsList, QObject *parent)
    : QObject{parent},
    _model(&model),
    _channelsList(&channelsList)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

NewsManager::~NewsManager()
{
    curl_global_cleanup();
}


size_t NewsManager::writer(char *data, size_t size, size_t nmemb, void *userp) {
    std::string *writerData = static_cast<std::string*>(userp);
    if(writerData == NULL) {
        qCritical() << "writer: userp is NULL";
        return 0;
    }
    writerData->append(data, size * nmemb);
    return size * nmemb;
}

std::string NewsManager::generateFilename(const std::string &original_url) {
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 9999);
    int random_suffix = distrib(gen);

    std::string extension = ".jpg";
    size_t dot_pos = original_url.rfind('.');
    size_t slash_pos = original_url.rfind('/');


    if (dot_pos != std::string::npos && (slash_pos == std::string::npos || dot_pos > slash_pos) && (dot_pos + 1 < original_url.length())) {
        std::string potential_ext = original_url.substr(dot_pos);

        if (potential_ext.length() <= 5 && potential_ext.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.") == std::string::npos) {
            extension = potential_ext;
        }
    }

    std::stringstream ss;
    ss << "image_" << timestamp << "_" << random_suffix << extension;
    return ss.str();
}

std::string NewsManager::downloadImage(const std::string &image_url_to_download) {
    std::string local_file_path = "";
    std::string image_data_buffer = "";
    char curl_error_buffer_image[CURL_ERROR_SIZE];
    CURL *conn_image = NULL;
    CURLcode res;

    conn_image = curl_easy_init();
    if (conn_image == NULL) {
        qDebug() << "downloadImage: Failed to create CURL connection\n";
        return "";
    }

    curl_easy_setopt(conn_image, CURLOPT_ERRORBUFFER, curl_error_buffer_image);
    curl_easy_setopt(conn_image, CURLOPT_TIMEOUT, 3L);
    curl_easy_setopt(conn_image, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:140.0) Gecko/20100101 Firefox/140.0");
    curl_easy_setopt(conn_image, CURLOPT_URL, image_url_to_download.c_str());
    curl_easy_setopt(conn_image, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(conn_image, CURLOPT_WRITEFUNCTION, writer);
    curl_easy_setopt(conn_image, CURLOPT_WRITEDATA, &image_data_buffer);

    res = curl_easy_perform(conn_image);
    if (res != CURLE_OK) {
        qDebug() << "downloadImage: curl_easy_perform() failed:" << curl_easy_strerror(res);
    }
    curl_easy_cleanup(conn_image);

    if (image_data_buffer.empty()) {
        qDebug() << "downloadImage: Image data buffer is empty for" << image_url_to_download;
        return "";
    }

    std::string unique_filename = generateFilename(image_url_to_download);
    local_file_path = "/tmp/pvm/" + unique_filename;

    std::ofstream fp(local_file_path, std::ios::binary);

    if (fp.is_open()) {
        fp << image_data_buffer;
        fp.close();
        qDebug() << "Image saved to:" << local_file_path;
        return "file://" + local_file_path;
    } else {
        perror("Error opening file for writing");
        qDebug() << "downloadImage: Failed to open file for writing:" << local_file_path;
        return "";
    }
}

std::string NewsManager::extractImageUrl(std::string url) {
    std::string og_image_url = "";
    CURL *conn = NULL;
    CURLcode res;
    std::string curl_write_buffer = "";
    char curl_error_buffer[CURL_ERROR_SIZE];

    conn = curl_easy_init();
    if (conn == NULL) {
        qDebug() << "extractOgImageUrl: Failed to create CURL connection\n";
        return og_image_url;
    }

    curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, curl_error_buffer);
    curl_easy_setopt(conn, CURLOPT_URL, url.c_str());
    curl_easy_setopt(conn, CURLOPT_TIMEOUT, 3L);
    curl_easy_setopt(conn, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:140.0) Gecko/20100101 Firefox/140.0");
    curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
    curl_easy_setopt(conn, CURLOPT_WRITEDATA, &curl_write_buffer);

    res = curl_easy_perform(conn);
    if (res != CURLE_OK) {
        qDebug() << "extractImageUrl: curl_easy_perform() failed:" << curl_easy_strerror(res);
        curl_easy_cleanup(conn);
        return og_image_url;
    }
    curl_easy_cleanup(conn);


    static QRegularExpression re(R"(<meta(?=[^>]*property\s*=\s*"og:image")(?=[^>]*content\s*=\s*"([^"]*)\")[^>]*>)", QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch i = re.match(QString::fromStdString(curl_write_buffer), 0, QRegularExpression::PartialPreferFirstMatch);

    if (i.hasMatch()) {
        og_image_url = i.captured(1).toStdString();
    }


    return og_image_url;
}

void NewsManager::parseRssItemThread(ParseItemData &itemData) {
    std::string og_image_url = "";
    const int MAX_RETRIES = 3;
    if (itemData.imageUrl.empty()){
        for (int i = 0; i < MAX_RETRIES; ++i) {
            og_image_url = extractImageUrl(itemData.link);
            if (!og_image_url.empty()) {
                qDebug() << "Found Open Graph image URL on attempt" << i + 1 << "for link:" << itemData.link;
                break;
            }
            qDebug() << "Attempt" << i + 1 << ": No Open Graph image URL found for link:" << itemData.link << ". Retrying...";
        }
    } else {
        og_image_url = itemData.imageUrl;
    }

    if (!og_image_url.empty()) {
        for (int i = 0; i < MAX_RETRIES; ++i) {
            itemData.imageUrl = downloadImage(og_image_url);
            if (!itemData.imageUrl.empty()) {
                qDebug() << "Downloaded image on attempt" << i + 1 << "for link:" << itemData.link;
                break;
            }
            qDebug() << "Attempt"<< i + 1 << ": No image downloaded for link:" << itemData.link << ". Retrying...";
        }
    } else {
        itemData.imageUrl = "";
        qDebug() <<  "No image found after" << MAX_RETRIES << "attempts for link:" << itemData.link;
    }
}


void NewsManager::rssWork(const std::string &url) {
    std::string urlCopy = url;

    QFuture future = QtConcurrent::run([this, urlCopy]() {

        CURL *curl_handle = NULL;
        CURLcode res;
        std::string rss_content_buffer;
        char curl_error_buffer_main[CURL_ERROR_SIZE];
        std::vector<ParseItemData> itemsToProcess;

        curl_handle = curl_easy_init();
        if (curl_handle) {
            curl_easy_setopt(curl_handle, CURLOPT_URL, urlCopy.c_str());
            curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writer);
            curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &rss_content_buffer);
            curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, curl_error_buffer_main);
            curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 3L);
            curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:140.0) Gecko/20100101 Firefox/140.0");


    
            res = curl_easy_perform(curl_handle);
    
            if (res != CURLE_OK) {
                qDebug() << "rssWork: curl_easy_perform() failed:" << curl_easy_strerror(res);
                itemsToProcess.push_back(ParseItemData {curl_easy_strerror(res), "err", "err"});
            }
            curl_easy_cleanup(curl_handle);
        } else {
            qDebug() << "rssWork: curl_easy_init() failed";
            itemsToProcess.push_back(ParseItemData {"curl_easy_init() failed", "err", "err"});
        }


        if (!rss_content_buffer.empty()) {
    

            QXmlStreamReader xml(QString::fromStdString(rss_content_buffer));

            if (xml.readNextStartElement()) {
                if (xml.name() == "rss") {
                    while (xml.readNextStartElement()) {
                        if (xml.name() == "channel") {
                            while (xml.readNextStartElement()) {
                                if (xml.name() == "item") {
                                    ParseItemData item;
                                    while (xml.readNextStartElement()) {
                                        if (xml.name() == "title") {
                                            item.title = xml.readElementText().toStdString();
                                        } else if (xml.name() == "link") {
                                            item.link = xml.readElementText().toStdString();
                                        } else if (xml.name() == "enclosure") {
                                            if (xml.attributes().hasAttribute("url") && xml.attributes().hasAttribute("type")) {
                                                QString type = xml.attributes().value("type").toString();
                                                if (type.startsWith("image/", Qt::CaseInsensitive)) {
                                                    item.imageUrl = xml.attributes().value("url").toString().toStdString();
                                                }
                                            }
                                            xml.skipCurrentElement();
                                        } else {
                                            xml.skipCurrentElement();
                                        }
                                    }
                                    if (!item.link.empty()) {
                                        itemsToProcess.push_back(item);
                                    }
                                } else {
                                    xml.skipCurrentElement();
                                }
                            }
                        } else {
                            xml.skipCurrentElement();
                        }
                    }
                } else {
                    qWarning() << "Root element is not 'rss'. Attempting to parse as RSS anyway.";
                }
            }

            if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {
                qCritical() << "XML parsing error:" << xml.errorString();
            }

            std::vector<std::thread> threads;
            threads.reserve(itemsToProcess.size());

            for (ParseItemData &item : itemsToProcess) {
                std::thread thread(parseRssItemThread, std::ref(item));
                threads.push_back(std::move(thread));

            }
            for (std::thread &th : threads) {
                th.join();
            }
        }


        QMetaObject::invokeMethod(this, [this, itemsToProcess]() {
            if (!itemsToProcess.empty()) {
                for (const auto& article : itemsToProcess) {
                    // if (article.imageUrl != "")
                    _model->addData(VElement(QString::fromStdString(article.title), QString::fromStdString(article.imageUrl), QString::fromStdString(article.link)));
                }
            } else {
                qDebug() << "Empty";
                _model->addData(VElement("RSS Parse failed", "", "lnk"));
            }
        }, Qt::QueuedConnection);
        loading = false;
        emit isLoading(false);
    });
}

void NewsManager::parseNews(const QString &url, const int index)
{
    qDebug() << url;
    if (!loading){
        if (url != currentURL && url != "add"){
            currentIndex = index;
            loading = true;
            emit isLoading(true);
            currentURL = url;
            QDir tempdir(QDir::cleanPath(QDir::tempPath() + QDir::separator() + "pvm"));
            if (tempdir.exists()){
                tempdir.removeRecursively();
                if (!tempdir.mkpath(".")){
                    qDebug() << "TMP dir creation failed.";
                }
            } else {
                tempdir.mkpath(".");
            }
            _model->clearData();
            rssWork(url.toStdString());
        } else if (url == "add"){
            _model->addData(VElement(QTranslator::tr("Add channel to view news."), "", ""));
            emit isLoading(false);
            loading = false;
        } else if (url == currentURL) {
            currentIndex = index;
        }
    }
}

void NewsManager::addChannel(const QString &label, const QString &url)
{
    int channels_count = channels.beginReadArray("channels");
    channels.endArray();
    channels.beginWriteArray("channels");
    channels.setArrayIndex(channels_count);
    channels.setValue("label", label);
    channels.setValue("url", url);
    channels.endArray();
    channels.sync();
}

void NewsManager::reloadChannels()
{
    int size = channels.beginReadArray("channels");
    _channelsList->clearData();
    for (int i = 0; i < size; ++i) {
        channels.setArrayIndex(i);
        _channelsList->addData(VElement(channels.value("label").toString(), "", channels.value("url").toString()));

    }
    channels.endArray();
    _channelsList->addData(VElement(QTranslator::tr("Add channel"), "", "add"));
}


QString NewsManager::parseFirst(){
    QModelIndex index = _channelsList->index(0, 0, QModelIndex());
    parseNews(_channelsList->data(index, VListModel::LinkRole).toString(), 0);
    return _channelsList->data(index, VListModel::LabelRole).toString();
}

QString NewsManager::parseLast(){
    QModelIndex index = _channelsList->index(_channelsList->rowCount(QModelIndex()) - 2, 0, QModelIndex());
    parseNews(_channelsList->data(index, VListModel::LinkRole).toString(), _channelsList->rowCount(QModelIndex()) - 1);
    return _channelsList->data(index, VListModel::LabelRole).toString();
}


void NewsManager::reloadNews()
{
    if (!loading){
        if (currentURL != "add"){
            emit isLoading(true);
            loading = true;
            QDir tempdir(QDir::cleanPath(QDir::tempPath() + QDir::separator() + "pvm"));
            if (tempdir.exists()){
                tempdir.removeRecursively();
                if (!tempdir.mkpath(".")){
                    qDebug() << "TMP dir creation failed.";
                }
            } else {
                tempdir.mkpath(".");
            }
            _model->clearData();
            rssWork(currentURL.toStdString());
        } else if (currentURL == "add"){
            _model->addData(VElement(QTranslator::tr("Add channel to view news."), "", ""));
        }
    }
}


QString NewsManager::removeChannel(){
    if (currentURL != "add"){
        int size = channels.beginReadArray("channels");
        std::vector<QString> channelsLabels;
        std::vector<QString> channelsURLs;
        for (int i = 0; i < size; ++i) {
            if (i != currentIndex) {
                channels.setArrayIndex(i);
                channelsLabels.push_back(channels.value("label").toString());
                channelsURLs.push_back(channels.value("url").toString());
            }
        }
        channels.endArray();

        channels.beginGroup("channels");
        channels.remove("");
        channels.endGroup();

        channels.beginWriteArray("channels");
        for (unsigned long i = 0; i < channelsLabels.size(); i++){
            channels.setArrayIndex(i);
            channels.setValue("label", channelsLabels.at(i));
            channels.setValue("url", channelsURLs.at(i));
        }
        channels.endArray();
        channels.sync();
        _model->clearData();
        reloadChannels();
        return parseFirst();
    }
    return "";
}
