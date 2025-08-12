#ifndef NEWSMANAGER_H
#define NEWSMANAGER_H

#include <QObject>
#include <QRegularExpression>
#include "vlistmodel.h"

struct ParseItemData {
    std::string title;
    std::string link;
    std::string imageUrl;
};


class NewsManager : public QObject
{
    Q_OBJECT

public:
    explicit NewsManager(VListModel &model, VListModel &channelsList, QObject *parent = nullptr);
    ~NewsManager();

    void rssWork(const std::string& url);
    Q_INVOKABLE void parseNews(const QString& url);
    Q_INVOKABLE void addChannel(const QString& label,  const QString& url);
    Q_INVOKABLE void reloadChannels();
    Q_INVOKABLE QString parseFirst();
    Q_INVOKABLE QString parseLast();
    Q_INVOKABLE void reloadNews();

private:
    // static QRegularExpression re;
    static size_t writer(char *data, size_t size, size_t nmemb, void *userp);

    static std::string generateFilename(const std::string& original_url);

    static std::string downloadImage(const std::string& image_url_to_download);

    static std::string extractImageUrl(std::string url);

    static void parseRssItemThread(ParseItemData &itemData);

    static std::vector<ParseItemData> parseRSS(std::string url);


    VListModel *_model;
    VListModel *_channelsList;
    QString currentURL;
    bool loading;
signals:
    void isLoading(bool);
};

#endif // NEWSMANAGER_H
