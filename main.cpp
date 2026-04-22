#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "emulationcontroller.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    EmulationController controller;

    QQmlApplicationEngine engine;

    // Делаем контроллер доступным в QML под именем "controller"
    engine.rootContext()->setContextProperty("controller", &controller);

    const QUrl url(u"qrc:/untitled4/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
