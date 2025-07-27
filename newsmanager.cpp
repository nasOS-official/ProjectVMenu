#include "newsmanager.h"
#include "imageparser.h"
#include "vlistmodel.h"
#include <QDir>
#include <QtConcurrent/QtConcurrentRun>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QTranslator>

NewsManager::NewsManager(VListModel &model, VListModel &channelsList, QObject *parent)
    : QObject{parent},
    _model(&model),
    _channelsList(&channelsList)
{}

void NewsManager::rssWork(const std::string &url) {
    std::string urlCopy = url;

    QtConcurrent::run([this, urlCopy]() {
        std::vector<std::vector<std::string>> articles = parseRSS(urlCopy);

        QMetaObject::invokeMethod(this, [this, articles]() {
            if (!articles.empty()) {
                for (const auto& article : articles) {
                    _model->addData(VElement(QString::fromStdString(article[0]), QString::fromStdString(article[2]), QString::fromStdString(article[1])));
                }
            } else {
                qDebug() << "Empty";
                _model->addData(VElement("RSS Parse failed", "", "lnk"));
            }
        }, Qt::QueuedConnection);
        emit isLoading(false);
    });
}


void NewsManager::parseNews(const QString &url)
{

    if (url != currentURL & url != "add"){
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
        qDebug() << "hello add";
        _model->addData(VElement(QTranslator::tr("Add channel to view news."), "", ""));
        emit isLoading(false);
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


void NewsManager::parseFirst(){
    QModelIndex index = _channelsList->index(0, 0, QModelIndex());
    parseNews(_channelsList->data(index, VListModel::LinkRole).toString());
}
