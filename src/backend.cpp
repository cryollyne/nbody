#include <QCoreApplication>
#include "backend.h"
QDir Backend::DATADIR;

Backend::Backend(QObject *parent)
    : QObject(parent)
{
    QDir dir = QDir(QCoreApplication::applicationDirPath());
#ifdef __linux__
    dir.cdUp();
    dir.cd("share");
    dir.cd("nbody");
#endif
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__) && !defined(__CYGWIN__)
    dir.cd("data");
    dir.cd("nbody");
#endif
    Backend::DATADIR = dir;
}
