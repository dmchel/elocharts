#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "qml/qmlwrapper.h"
#include "coreserver.h"

const QString progVersion = "ver. 0.5";

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        qDebug() << "Couldn't load qml module. Exit now.";
        return -1;
    }
    QMLWrapper wrap = new QMLWrapper(engine.rootObjects().at(0));
    CoreServer server;



    return app.exec();
}
