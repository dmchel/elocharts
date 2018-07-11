#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class ChartWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setChartWidget(ChartWidget *widget);

private:
    Ui::MainWindow *ui;
    ChartWidget *charts = Q_NULLPTR;
};

#endif // MAINWINDOW_H
