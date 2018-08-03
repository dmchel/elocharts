#include <QtWidgets/QVBoxLayout>
#include <QDebug>

#include "chartwidget.h"

LiveChart::LiveChart(QChart *chart, QWidget *parent)
{
    setChart(chart);
    setParent(parent);
}

void LiveChart::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        fLeftButton = true;
    }
    event->accept();
}

void LiveChart::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        fLeftButton = false;
    }
    event->accept();
}

void LiveChart::mouseMoveEvent(QMouseEvent *event)
{
    if(!fLeftButton) {
        prevMousePos = event->pos();
        event->ignore();
    }
    else {
        if(prevMousePos.isNull()) {
            prevMousePos = event->pos();
        }
        else {
            QPoint point = event->pos() - prevMousePos;
            this->chart()->scroll(0, (-1) * (point.ry() / 10.0));
        }
    }
}

ChartWidget::ChartWidget(QWidget *parent) : QWidget(parent)
{
    mainChart = new QChart();
    liveChart = new LiveChart(mainChart);
    liveChart->setMinimumSize(640, 480);
    axisX = new QValueAxis;
    axisX->setRange(0, CHART_UPDATE_INTERVAL_MS);
    axisX->setLabelFormat("%g");
    axisX->setTitleText(tr("Время, мс"));
    axisY = new QValueAxis;
    axisY->setRange(0, 1000);
    mainChart->setAxisX(axisX);
    mainChart->setAxisY(axisY);
    mainChart->legend()->hide();
    mainChart->setTheme(QChart::ChartThemeDark);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(liveChart);
    setLayout(mainLayout);
}

ChartWidget::~ChartWidget()
{
    //delete mainChart;
}

void ChartWidget::mousePressEvent(QMouseEvent *event)
{
    liveChart->mousePressEvent(event);
}

void ChartWidget::mouseReleaseEvent(QMouseEvent *event)
{
    liveChart->mouseReleaseEvent(event);
}

void ChartWidget::mouseMoveEvent(QMouseEvent *event)
{
    liveChart->mouseMoveEvent(event);
}

void ChartWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        //m_chart->scroll(numPixels.ry(), numPixels.ry());
        if(numPixels.ry() > 0) {
            mainChart->zoomIn();
        }
        else if(numPixels.ry() < 0) {
            mainChart->zoomOut();
        }
    } else if (!numDegrees.isNull()) {
        //QPoint numSteps = numDegrees;
        //m_chart->scroll(numSteps.ry(), numSteps.ry());
        if(numDegrees.ry() > 0) {
            mainChart->zoomIn();
        }
        else if(numDegrees.ry() < 0) {
            mainChart->zoomOut();
        }
    }
    event->accept();
}

void ChartWidget::addDataToChart(int id, qreal x, qreal y)
{
    if(seriesMap.contains(id)) {
        seriesMap.value(id)->append(x, y);
    }
    else {
        seriesMap.insert(id, createNewLineSeries());
        seriesMap.value(id)->append(x, y);
    }
    updateVerticalAxisRange(y);
    qreal dx = x - prevPoint.rx();
    intervalChartTime += dx;
    if(intervalChartTime > CHART_UPDATE_INTERVAL_MS) {
        mainChart->scroll(mainChart->rect().width() - 100, 0);
        intervalChartTime = 0;
    }
    prevPoint.setX(x);
    mainChart->update();
}

void ChartWidget::removeDataFromChart(int id, qreal x, qreal y)
{
    if(seriesMap.contains(id)) {
        seriesMap.value(id)->remove(x, y);
        mainChart->update();
    }
}

void ChartWidget::clearChart()
{
    for(auto series : seriesMap) {
        series->clear();
    }
    mainChart->update();
    prevPoint = QPointF(0.0, 0.0);
}

/**
 * Private methods
 */

QLineSeries *ChartWidget::createNewLineSeries()
{
    QLineSeries *ls = new QLineSeries();
    mainChart->addSeries(ls);
    ls->attachAxis(axisX);
    ls->attachAxis(axisY);
    return ls;
}

void ChartWidget::updateVerticalAxisRange(qreal y)
{
    if(y > axisY->max()) {
        axisY->setMax(y * 1.05);
    }
    else if(y < axisY->min()) {
        axisY->setMin(y * 1.05);
    }
}
