#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chartwidget.h"
#include "eloplotter.h"

#include "createparamdialog.h"
#include "recordModel/chartrecordmodel.h"
#include "../console.h"

#include <QTableView>
#include <QDockWidget>
#include <QMessageBox>

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
    //ui->addParamAction->setDisabled(true);
    connect(ui->addParamAction, &QAction::triggered, this, &MainWindow::addNewParam);
    console = new Console(this);
    console->setLocalEchoEnabled(true);
    console->hide();
    connect(console, &Console::closed, this, &MainWindow::onCloseConsole);
    connect(console, &Console::eSendCommand, this, &MainWindow::sendShellCommand);

    ui->buttonDockWidget->setTitleBarWidget(new QWidget());

    connect(ui->runAction, &QAction::triggered, this, &MainWindow::runPlot);
    connect(ui->stopAction, &QAction::triggered, this, &MainWindow::pausePlot);
    connect(ui->resetAction, &QAction::triggered, this, &MainWindow::resetPlot);
    connect(ui->fitInAction, &QAction::triggered, this, &MainWindow::fitInPlots);
    connect(ui->runButton, &QPushButton::clicked, this, &MainWindow::runPlot);
    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::pausePlot);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::resetPlot);
    connect(ui->fitInButton, &QPushButton::clicked, this, &MainWindow::fitInPlots);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setChartWidget(QWidget *widget)
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
        varTable->hideColumn(0);
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
        connectionStatusLabel->setText(tr("Соединение установлено   "));
    }
    else {
        connectionStatusLabel->setText(tr("Нет связи с устройством   "));
    }
}

void MainWindow::updateConnectionInfo(const QString &str)
{
    connectionInfoLabel->setText(str + QString("     "));
}

void MainWindow::showStatusMessage(const QString &str)
{
    this->statusBar()->showMessage(str);
}

void MainWindow::consolePrintText(const QString &str)
{
    console->putString(str);
}

void MainWindow::consoleUnknownCmd(const QString &str)
{
    console->hCommandError(str);
}

void MainWindow::consolePutData(const QByteArray &data)
{
    console->putData(data);
}

void MainWindow::consoleClear()
{
    console->clear();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space:
        if(fRun) {
            emit pausePlot();
        }
        else {
            emit runPlot();
        }
        fRun = !fRun;
        break;
    case Qt::Key_F12:
        if(console->isVisible()) {
            console->hide();
        }
        else {
            console->show();
            console->raise();
            console->setFocus();
        }
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit aboutToClose();
    event->accept();
}

/**
 * Private methods
 */

void MainWindow::onCloseConsole()
{
    this->setFocus();
}

void MainWindow::addNewParam()
{
    CreateParamDialog paramDialog(this);
    if(paramDialog.exec() == QDialog::Accepted) {
        QJsonObject jsonObject = paramDialog.jsonData();
        if(!jsonObject.isEmpty()) {
            if(jsonObject["name"].toString().isEmpty()) {
                QMessageBox::warning(this, tr("Предупреждение"), tr("Необходимо ввести имя параметра."), QMessageBox::Ok);
            }
            else {
                emit sendParamData(jsonObject);
            }
        }
        else {
            QMessageBox::critical(this, tr("Ошибка"), tr("Произошла ошбика чтения параметра!"), QMessageBox::Ok);
        }
    }
}
