#include <QApplication>
#include <QDebug>

#include "qml/qmlwrapper.h"
#include "gui/mainwindow.h"
#include "gui/chartwidget.h"
#include "gui/eloplotter.h"
#include "coreserver.h"
#include "shell.h"

const QString progVersion = QString("ver. 0.7.0.0 " + QString(__DATE__) + QString(" ") + QString(__TIME__));

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    Shell shell;
    MainWindow w;
    ChartWidget chart;
    ELOPlotter plot;

    CoreServer server;
    server.setSoftVersion(progVersion);
    //server connections
    //QObject::connect(&server, &CoreServer::chartData, &chart, &ChartWidget::addDataToChart);
    QObject::connect(&server, &CoreServer::chartData, &plot, &ELOPlotter::addDataToPlot);
    QObject::connect(&server, &CoreServer::sendConnectionStatus, &w, &MainWindow::updateConnectionStatus);
    QObject::connect(&server, &CoreServer::connectionInfoChanged, &w, &MainWindow::updateConnectionInfo);
    QObject::connect(&server, &CoreServer::sendUartRxData, &w, &MainWindow::consolePutData);
    QObject::connect(&server, &CoreServer::sendUartRxData, &shell, &Shell::receiveData);
    QObject::connect(&server, &CoreServer::sendConsoleText, &w, &MainWindow::consolePrintText);
    //shell connections
    QObject::connect(&w, &MainWindow::sendShellCommand, &shell, &Shell::hNewCommand);
    QObject::connect(&shell, &Shell::ePrintString, &w, &MainWindow::consolePrintText);
    QObject::connect(&shell, &Shell::eUnknownCommand, &w, &MainWindow::consoleUnknownCmd);
    QObject::connect(&shell, &Shell::eClear, &w, &MainWindow::consoleClear);
    QObject::connect(&shell, &Shell::eListenUart, &server, &CoreServer::shellListenUart);
    QObject::connect(&shell, &Shell::eStatusUart, &server, &CoreServer::shellUartStatus);
    QObject::connect(&shell, &Shell::eSendUart, &server, &CoreServer::shellSendUart);
    QObject::connect(&shell, &Shell::ePortList, &server, &CoreServer::shellPrintPorts);
    QObject::connect(&shell, &Shell::eVersionRequest, &server, &CoreServer::shellVersionRequest);

    w.setChartWidget(&plot);
    w.setTableModel(server.dataModel());
    w.setTableDelegate(server.dataDelegate());
    w.show();
    //plot.show();

    return app.exec();
}
