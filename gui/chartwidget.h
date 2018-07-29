#ifndef GUICHARTWIDGET_H
#define GUICHARTWIDGET_H

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>

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
    void addDataToChart(int id, qreal x, qreal y);
    void removeDataFromChart(int id, qreal x, qreal y);
    void clearChart();

private:
    QLineSeries *createNewLineSeries();
    void updateVerticalAxisRange(qreal y);

private:
    LiveChart *liveChart = Q_NULLPTR;
    QChart *mainChart = Q_NULLPTR;
    QMap<int, QLineSeries *> seriesMap;
    QValueAxis *axisX = Q_NULLPTR;
    QValueAxis *axisY = Q_NULLPTR;
    QLineSeries *m_series = Q_NULLPTR;
    QPointF prevPoint = QPoint(0.0, 0.0);
    qreal intervalChartTime = 0.0;

    const qreal CHART_UPDATE_INTERVAL_MS = 10000.0;
};

#endif // GUICHARTWIDGET_H
