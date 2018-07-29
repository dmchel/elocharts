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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setChartWidget(ChartWidget *widget);
    void setTableModel(QAbstractItemModel *model);
    void setTableDelegate(QAbstractItemDelegate *delegate);

public slots:
    void updateConnectionStatus(bool flag);
    void updateConnectionInfo(const QString &str);
    void showStatusMessage(const QString &str);

private:
    Ui::MainWindow *ui;
    QDockWidget *bottomDockWidget = Q_NULLPTR;
    ChartWidget *charts = Q_NULLPTR;
    QTableView *varTable = Q_NULLPTR;
    QLabel *connectionStatusLabel = Q_NULLPTR;
    QLabel *connectionInfoLabel = Q_NULLPTR;
};

#endif // MAINWINDOW_H
