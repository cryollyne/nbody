#include <QCoreApplication>
#include "backend.h"
QDir Backend::DATADIR;

Backend::Backend(QObject *parent)
    : QObject(parent)
{
    QDir dir = QDir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("share");
    dir.cd("nbody");
    Backend::DATADIR = dir;
}
