#include "vrunner.h"
#include "vlistmodel.h"
#include <QDir>

VRunner::VRunner(VListModel &model, QObject *parent)
    : QObject{parent},
    _model(&model)
{
    QObject::connect(&process, &QProcess::stateChanged,
                     this, &VRunner::handlerStateChanged);
    scanApps();
}

VRunner::~VRunner()
{

}

void VRunner::handlerStateChanged(QProcess::ProcessState state)
{
    if (state == 0){
        _model->setRunning(_currentIndex, false);
    } else {
        _model->setRunning(_currentIndex, true);
    }

    emit stateChanged(state);
}



void VRunner::start(const QString &execs, int index)
{
    if (process.processId() == 0) {
        _currentIndex = index;
        process.start(execs);
    }
}


QString VRunner::currentApp() const
{
    if (process.processId() != 0) {
        QModelIndex index = _model->index(_currentIndex, 0, QModelIndex());
        return _model->data(index, VListModel::LabelRole).toString();
    }
    return "";
}

void VRunner::terminate()
{
    if (process.processId() != 0) {
        process.kill();
        process.waitForFinished();
    }
}

void VRunner::startRequest(const QString &execs, int index)
{
    _requestExec = execs;
    _requestIndex = index;
}

void VRunner::acceptStartRequest()
{
    qDebug() << "starting...";
    start(_requestExec, _requestIndex);
}

bool VRunner::isRunning() const
{
    return process.processId() == 0 ? false : true;
}

void VRunner::scanApps() {
    QDir applicationsDir("/usr/share/applications/");
    if (!applicationsDir.exists()) {
        qWarning() << "Directory /usr/share/applications/ does not exist.";
        return;
    }

    QStringList filters;
    filters << "*.desktop";
    QFileInfoList desktopFiles = applicationsDir.entryInfoList(filters, QDir::Files | QDir::Readable);

    for (const QFileInfo& fileInfo : std::as_const(desktopFiles)) {
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Could not open file for reading:" << fileInfo.absoluteFilePath();
            continue;
        }

        QTextStream in(&file);
        QString name;
        QString execute;
        QString tryexecute;
        QString icon;
        QString currentSection;
        bool desktopEntryFound = false;

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();

            if (line.startsWith("[")) {
                currentSection = line;
                if (currentSection == "[Desktop Entry]") {
                    desktopEntryFound = true;
                }
            } else if (desktopEntryFound && currentSection == "[Desktop Entry]") {
                int equalsIndex = line.indexOf('=');
                if (equalsIndex > 0) {
                    QString key = line.left(equalsIndex);
                    QString value = line.mid(equalsIndex + 1);

                    if (key == "Name") {
                        name = value;
                    } else if (key == "Exec") {
                        execute = value;
                    } else if (key == "TryExec") {
                        tryexecute = value;
                    } else if (key == "Icon") {
                        icon = value;
                    }
                }
            }
        }
        file.close();

        if (desktopEntryFound) {
            _model->addData(VElement(name, "file://" + icon, tryexecute.isEmpty() ? execute : tryexecute));
        }
    }
}


