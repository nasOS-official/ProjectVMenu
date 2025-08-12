#include "vrunner.h"
#include "vlistmodel.h"


VRunner::VRunner(VListModel &model, QObject *parent)
    : QObject{parent},
    _model(&model)
{
    QObject::connect(&process, &QProcess::stateChanged,
                     this, &VRunner::handlerStateChanged);
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


