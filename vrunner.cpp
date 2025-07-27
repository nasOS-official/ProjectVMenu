#include "vrunner.h"

VRunner::VRunner(QObject *parent)
    : QObject{parent}
{}

void VRunner::buttonClicked(const QString &execs)
{
    process.start(execs);
}
