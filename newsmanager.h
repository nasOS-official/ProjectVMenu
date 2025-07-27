#ifndef NEWSMANAGER_H
#define NEWSMANAGER_H

#include <QObject>
#include "vlistmodel.h"

class NewsManager : public QObject
{
    Q_OBJECT
public:
    explicit NewsManager(VListModel &model, VListModel &channelsList, QObject *parent = nullptr);
    void rssWork(const std::string& url);

public slots:
    void parseNews(const QString& url);
    void addChannel(const QString& label,  const QString& url);
    void reloadChannels();
    void parseFirst();
private:
    VListModel *_model;
    VListModel *_channelsList;
    QString currentURL;
signals:
    void isLoading(bool);
};

#endif // NEWSMANAGER_H
