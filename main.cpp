#include <QApplication>
#include <QDebug>

#include "qml/qmlwrapper.h"
#include "gui/mainwindow.h"
#include "gui/chartwidget.h"
#include "coreserver.h"

const QString progVersion = "ver. 0.6.0.0";

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    MainWindow w;
    ChartWidget chart;
    //chart.setMouseTracking(true);
    CoreServer server;

    QObject::connect(&server, &CoreServer::chartData, &chart, &ChartWidget::addDataToChart);
    QObject::connect(&server, &CoreServer::sendConnectionStatus, &w, &MainWindow::updateConnectionStatus);
    QObject::connect(&server, &CoreServer::connectionInfoChanged, &w, &MainWindow::updateConnectionInfo);

    w.setChartWidget(&chart);
    w.setTableModel(server.dataModel());
    w.setTableDelegate(server.dataDelegate());
    w.show();
    chart.show();

    return app.exec();
}
