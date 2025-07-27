#ifndef VRUNNER_H
#define VRUNNER_H

#include <QObject>
#include <QProcess>

class VRunner : public QObject
{
    Q_OBJECT
public:
    explicit VRunner(QObject *parent = nullptr);

private:
    QProcess process;

public slots:
    void buttonClicked(const QString& exec);
};

#endif // VRUNNER_H
