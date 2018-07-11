#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chartwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setChartWidget(ChartWidget *widget)
{
    if(widget) {
        charts = widget;
        //ui->chartWidget = charts;
    }
}
