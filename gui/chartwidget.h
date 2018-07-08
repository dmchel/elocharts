#ifndef GUICHARTWIDGET_H
#define GUICHARTWIDGET_H

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

/*class LiveChart : public QChart
{
    Q_OBJECT

public:
    LiveChart(QObject *parent = 0);
    ~LiveChart();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

};*/

class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    ChartWidget(QWidget *parent = 0);
    ~ChartWidget();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

public slots:
    void addDataToChart(qreal x, qreal y);
    void removeDataFromChart(qreal x, qreal y);
    void clearChart();

private:
    QChart *m_chart = Q_NULLPTR;
    QLineSeries *m_series = Q_NULLPTR;
    QPoint prevMousePos = QPoint(0, 0);
    bool fLeftButton = false;
    QPointF prevPoint = QPoint(0.0, 0.0);
};

#endif // GUICHARTWIDGET_H
