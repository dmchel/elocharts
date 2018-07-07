#ifndef GUICHARTWIDGET_H
#define GUICHARTWIDGET_H

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    ChartWidget(QWidget *parent = 0);
    ~ChartWidget();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

public slots:
    void addDataToChart(qreal x, qreal y);
    void removeDataFromChart(qreal x, qreal y);
    void clearChart();

private:
    QChart *m_chart;
    QLineSeries *m_series;
};

#endif // GUICHARTWIDGET_H
