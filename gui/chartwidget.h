#ifndef GUICHARTWIDGET_H
#define GUICHARTWIDGET_H

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

QT_CHARTS_USE_NAMESPACE

class LiveChart : public QChartView
{
    Q_OBJECT

public:
    LiveChart(QChart *chart, QWidget *parent = Q_NULLPTR);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QPoint prevMousePos = QPoint(0, 0);
    bool fLeftButton = false;

};

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
    LiveChart *liveChart = Q_NULLPTR;
    QChart *mainChart = Q_NULLPTR;
    QList<QLineSeries> seriesList;
    QLineSeries *m_series = Q_NULLPTR;
    QPointF prevPoint = QPoint(0.0, 0.0);
    qreal totalChartTime = 0.0;
};

#endif // GUICHARTWIDGET_H
