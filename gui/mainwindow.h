#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class QTableView;
class QAbstractItemModel;
class QAbstractItemDelegate;
class ChartWidget;
class Console;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setChartWidget(QWidget *widget);
    void setTableModel(QAbstractItemModel *model);
    void setTableDelegate(QAbstractItemDelegate *delegate);
    void setApplicationVersion(const QString &str);

    void closeEvent(QCloseEvent *event);

signals:
    void aboutToClose();
    void sendShellCommand(const QString &cmd);
    void sendParamData(const QJsonObject &data);
    //plot control signals
    void runPlot();
    void pausePlot();
    void fitInPlots();
    void resetPlot();

public slots:
    void updateConnectionStatus(bool flag);
    void updateConnectionInfo(const QString &str);
    void showStatusMessage(const QString &str);
    void updateTableGeometry();
    //console interface
    void consolePrintText(const QString &str);
    void consoleUnknownCmd(const QString &str);
    void consolePutData(const QByteArray &data);
    void consoleClear();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void onCloseConsole();
    void addNewParam();
    void about();

private:
    Ui::MainWindow *ui;
    QDockWidget *bottomDockWidget = Q_NULLPTR;
    QWidget *charts = Q_NULLPTR;
    QTableView *varTable = Q_NULLPTR;
    QLabel *connectionStatusLabel = Q_NULLPTR;
    QLabel *connectionInfoLabel = Q_NULLPTR;
    Console *console;

    bool fRun = true;
    QString appVersion = "";
};

#endif // MAINWINDOW_H
