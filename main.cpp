#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "superbinding.h"
#include "slowbackend.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<SuperBinding>("SuperBinding", 1, 0, "SuperBinding");
    qmlRegisterType<SlowBackend>("SlowBackend", 1, 0, "SlowBackend");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
