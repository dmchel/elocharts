#include <QApplication>
#include <QQmlApplicationEngine>
#include <QDebug>

#include "qml/qmlwrapper.h"
#include "gui/mainwindow.h"
#include "gui/chartwidget.h"
#include "coreserver.h"

const QString progVersion = "ver. 0.5";

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    /*QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        qDebug() << "Couldn't load qml module. Exit now.";
        return -1;
    }*/
    MainWindow w;
    ChartWidget chart;
    //chart.setMouseTracking(true);
    //QMLWrapper wrap(engine.rootObjects().at(0));
    CoreServer server;

    //QObject::connect(&server, &CoreServer::chartData, &wrap, &QMLWrapper::addDataToChart);
    QObject::connect(&server, &CoreServer::chartData, &chart, &ChartWidget::addDataToChart);

    w.setChartWidget(&chart);
    w.show();
    chart.show();

    return app.exec();
}
