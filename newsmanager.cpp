#include "newsmanager.h"

#include <string>
#include <vector>
#include <thread>
#include <curl/curl.h>
#include <chrono>
#include <random>
#include <sstream>
#include <iostream>


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
        fprintf(stderr, "writer: userp is NULL\n");
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
    std::string image_data_buffer;
    char curl_error_buffer_image[CURL_ERROR_SIZE];
    CURL *conn_image = NULL;
    CURLcode code_image;

    conn_image = curl_easy_init();
    if (conn_image == NULL) {
        fprintf(stderr, "download_and_save_image: Failed to create CURL connection\n");
        return "";
    }

    code_image = curl_easy_setopt(conn_image, CURLOPT_ERRORBUFFER, curl_error_buffer_image);
    if (code_image != CURLE_OK) {
        fprintf(stderr, "download_and_save_image: Failed to set error buffer [%d]: %s\n", code_image, curl_error_buffer_image);
        curl_easy_cleanup(conn_image);
        return "";
    }

    code_image = curl_easy_setopt(conn_image, CURLOPT_TIMEOUT, 3L);
    if (code_image != CURLE_OK) {
        fprintf(stderr, "download_and_save_image: Failed to set timeout option [%s]\n", curl_error_buffer_image);
        curl_easy_cleanup(conn_image);
        return "";
    }

    code_image = curl_easy_setopt(curl_error_buffer_image, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:140.0) Gecko/20100101 Firefox/140.0");
    if (code_image != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set UserAgent [%s]\n", curl_error_buffer_image);
        curl_easy_cleanup(conn_image);
        return "";
    }


    code_image = curl_easy_setopt(conn_image, CURLOPT_URL, image_url_to_download.c_str());
    if (code_image != CURLE_OK) {
        fprintf(stderr, "download_and_save_image: Failed to set URL '%s': %s\n", image_url_to_download.c_str(), curl_error_buffer_image);
        curl_easy_cleanup(conn_image);
        return "";
    }

    code_image = curl_easy_setopt(conn_image, CURLOPT_FOLLOWLOCATION, 1L);
    if (code_image != CURLE_OK) {
        fprintf(stderr, "download_and_save_image: Failed to set redirect option [%s]\n", curl_error_buffer_image);
        curl_easy_cleanup(conn_image);
        return "";
    }

    code_image = curl_easy_setopt(conn_image, CURLOPT_WRITEFUNCTION, writer);
    if (code_image != CURLE_OK) {
        fprintf(stderr, "download_and_save_image: Failed to set writer [%s]\n", curl_error_buffer_image);
        curl_easy_cleanup(conn_image);
        return "";
    }
    image_data_buffer.clear();
    code_image = curl_easy_setopt(conn_image, CURLOPT_WRITEDATA, &image_data_buffer);
    if (code_image != CURLE_OK) {
        fprintf(stderr, "download_and_save_image: Failed to set write data [%s]\n", curl_error_buffer_image);
        curl_easy_cleanup(conn_image);
        return "";
    }

    code_image = curl_easy_perform(conn_image);
    if (code_image != CURLE_OK) {
        fprintf(stderr, "download_and_save_image: Failed to download image from '%s': %s\n", image_url_to_download.c_str(), curl_error_buffer_image);
    }
    curl_easy_cleanup(conn_image);

    if (image_data_buffer.empty()) {
        fprintf(stderr, "download_and_save_image: Image data buffer is empty for '%s'\n", image_url_to_download.c_str());
        return "";
    }

    std::string unique_filename = generateFilename(image_url_to_download);
    local_file_path = "/tmp/pvm/" + unique_filename;

    FILE* fp = fopen(local_file_path.c_str(), "wb");
    if (fp) {
        fwrite(image_data_buffer.data(), 1, image_data_buffer.size(), fp);
        fclose(fp);
        fprintf(stderr, "Image saved to: %s\n", local_file_path.c_str());
        return "file://" + local_file_path;
    } else {
        perror("Error opening file for writing");
        fprintf(stderr, "download_and_save_image: Failed to open file for writing: %s\n", local_file_path.c_str());
        return "";
    }
}

std::string NewsManager::extractImageUrl(std::string url) {
    std::string og_image_url = "";
    CURL *conn = NULL;
    CURLcode code;
    std::string curl_write_buffer;
    char curl_error_buffer[CURL_ERROR_SIZE];

    conn = curl_easy_init();
    if (conn == NULL) {
        fprintf(stderr, "extractOgImageUrl: Failed to create CURL connection\n");
        return og_image_url;
    }

    code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, curl_error_buffer);
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set error buffer [%d]: %s\n", code, curl_error_buffer);
        curl_easy_cleanup(conn);
        return og_image_url;
    }

    code = curl_easy_setopt(conn, CURLOPT_URL, url.c_str());
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set URL [%s]\n", curl_error_buffer);
        curl_easy_cleanup(conn);
        return og_image_url;
    }

    code = curl_easy_setopt(conn, CURLOPT_TIMEOUT, 3L);
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set timeout option [%s]\n", curl_error_buffer);
        curl_easy_cleanup(conn);
        return og_image_url;
    }

    code = curl_easy_setopt(conn, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:140.0) Gecko/20100101 Firefox/140.0");
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set UserAgent [%s]\n", curl_error_buffer);
        curl_easy_cleanup(conn);
        return og_image_url;
    }

    code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set redirect option [%s]\n", curl_error_buffer);
        curl_easy_cleanup(conn);
        return og_image_url;
    }

    code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set writer [%s]\n", curl_error_buffer);
        curl_easy_cleanup(conn);
        return og_image_url;
    }

    curl_write_buffer.clear();
    code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &curl_write_buffer);
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set write data [%s]\n", curl_error_buffer);
        curl_easy_cleanup(conn);
        return og_image_url;
    }

    code = curl_easy_perform(conn);
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to get '%s': %s\n", url.c_str(), curl_error_buffer);
    }

    static QRegularExpression re(R"(<meta(?=[^>]*property\s*=\s*"og:image")(?=[^>]*content\s*=\s*"([^"]*)\")[^>]*>)", QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch i = re.match(QString::fromStdString(curl_write_buffer), 0, QRegularExpression::PartialPreferFirstMatch);

    if (i.hasMatch()) {
        og_image_url = i.captured(1).toStdString();
    }

    curl_easy_cleanup(conn);

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



std::vector<ParseItemData> NewsManager::parseRSS(std::string url) {
    std::vector<ParseItemData> final_arr;
    CURL *curl_handle = NULL;
    CURLcode res;
    std::string rss_content_buffer;
    char curl_error_buffer_main[CURL_ERROR_SIZE];

    curl_handle = curl_easy_init();
    if (curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writer);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &rss_content_buffer);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, curl_error_buffer_main);

        res = curl_easy_perform(curl_handle);

        if (res != CURLE_OK) {
            fprintf(stderr, "parseRSS: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl_handle);
    } else {
        fprintf(stderr, "parseRSS: curl_easy_init() failed\n");
    }

    if (rss_content_buffer.empty()) {
        fprintf(stderr, "parseRSS: Failed to retrieve RSS content or content is empty.\n");
        return final_arr;
    }


    std::vector<ParseItemData> itemsToProcess;

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


    for (const auto& item : itemsToProcess) {
        if (item.imageUrl != ""){
            final_arr.push_back({item.title, item.link, item.imageUrl});
        }
    }

    return final_arr;
}



void NewsManager::rssWork(const std::string &url) {
    std::string urlCopy = url;

    QFuture future = QtConcurrent::run([this, urlCopy]() {
        std::vector<ParseItemData> articles = parseRSS(urlCopy);

        QMetaObject::invokeMethod(this, [this, articles]() {
            if (!articles.empty()) {
                for (const auto& article : articles) {
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

void NewsManager::parseNews(const QString &url)
{
    qDebug() << url;
    if (url != currentURL && url != "add"){
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
    }
}

void NewsManager::addChannel(const QString &label, const QString &url)
{
    QSettings channels("channels.cfg", QSettings::IniFormat);
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
    QSettings channels("channels.cfg", QSettings::IniFormat);
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
    parseNews(_channelsList->data(index, VListModel::LinkRole).toString());
    return _channelsList->data(index, VListModel::LabelRole).toString();
}

QString NewsManager::parseLast(){
    QModelIndex index = _channelsList->index(_channelsList->rowCount(QModelIndex()) - 2, 0, QModelIndex());
    parseNews(_channelsList->data(index, VListModel::LinkRole).toString());
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
