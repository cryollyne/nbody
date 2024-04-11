#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>
#include <QQuickStyle>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <qt/QtQuick/QQuickItem>

#include "canvas.h"
#include "backend.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("nbody");
    QCoreApplication::setApplicationName(QStringLiteral("Gravity Simulator"));

    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }


    QQmlApplicationEngine engine;
    Backend backend;
    qmlRegisterSingletonInstance<Backend>("App", 1, 0, "Backend", &backend);
    qmlRegisterType<Canvas>("Renderer", 1, 0, "Canvas");


    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}

