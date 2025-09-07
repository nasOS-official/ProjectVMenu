#ifndef VRUNNER_H
#define VRUNNER_H

#include <QObject>
#include <QProcess>
#include "vlistmodel.h"

class VRunner : public QObject
{
    Q_OBJECT
public:
    explicit VRunner(VListModel &model, QObject *parent = nullptr);
    ~VRunner();

    Q_INVOKABLE void start(const QString &execs, int index);
    Q_INVOKABLE bool isRunning() const;
    Q_INVOKABLE QString currentApp() const;
    Q_INVOKABLE void terminate();
    Q_INVOKABLE void startRequest(const QString &execs, int index);
    Q_INVOKABLE void acceptStartRequest();

private:
    void scanApps();
    QProcess process;
    int _currentIndex;
    int _requestIndex;
    QString _requestExec;
    VListModel *_model;

private slots:
    void handlerStateChanged(QProcess::ProcessState state);

signals:
    void stateChanged(QProcess::ProcessState);

};

#endif // VRUNNER_H
