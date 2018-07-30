#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chartwidget.h"

#include "recordModel/chartrecordmodel.h"

#include <QTableView>
#include <QDockWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    varTable = new QTableView(this);
    connectionStatusLabel = new QLabel(this);
    connectionInfoLabel = new QLabel(this);
    this->statusBar()->addPermanentWidget(connectionStatusLabel);
    this->statusBar()->addPermanentWidget(connectionInfoLabel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setChartWidget(ChartWidget *widget)
{
    if(widget) {
        if(charts != Q_NULLPTR) {
            delete charts;
        }
        charts = widget;
        this->setCentralWidget(charts);
    }
}

void MainWindow::setTableModel(QAbstractItemModel *model)
{
    if(model) {
        varTable->setModel(model);
        ChartRecordModel *chartModel = static_cast<ChartRecordModel*>(model);
        connect(chartModel, &ChartRecordModel::recordChanged, varTable, QOverload<const QModelIndex&>::of(&QTableView::update));
        if(bottomDockWidget != Q_NULLPTR) {
            this->removeDockWidget(bottomDockWidget);
        }
        bottomDockWidget = new QDockWidget(this);
        bottomDockWidget->setWidget(varTable);
        bottomDockWidget->setTitleBarWidget(new QWidget());
        bottomDockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
        this->addDockWidget(Qt::BottomDockWidgetArea, bottomDockWidget, Qt::Horizontal);
    }
}

void MainWindow::setTableDelegate(QAbstractItemDelegate *delegate)
{
    if(delegate) {
        varTable->setItemDelegate(delegate);
    }
}

void MainWindow::updateConnectionStatus(bool flag)
{
    if(flag) {
        connectionStatusLabel->setText(tr("Соединение установлено"));
    }
    else {
        connectionStatusLabel->setText(tr("Нет связи с устройством"));
    }
}

void MainWindow::updateConnectionInfo(const QString &str)
{
    connectionInfoLabel->setText(str);
}

void MainWindow::showStatusMessage(const QString &str)
{
    this->statusBar()->showMessage(str);
}
