#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QDir>

class Backend : public QObject {
    Q_OBJECT
public:
    explicit Backend(QObject *parent = nullptr);

    static QDir DATADIR;
};

#endif // !BACKEND_H
