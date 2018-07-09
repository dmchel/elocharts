#include <QtWidgets/QVBoxLayout>
#include <QtCharts/QValueAxis>
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
        event->ignore();
    }
    else {
        if(prevMousePos.isNull()) {
            prevMousePos = event->pos();
        }
        else {
            QPoint point = event->pos() - prevMousePos;
            qreal dx = 0.0, dy = 0.0;
            if(point.rx() > 0) {
                dx = -1.0;
            }
            else if(point.rx() < 0) {
                dx = 1.0;
            }
            if(point.ry() > 0) {
                dy = -1.0;
            }
            else if(point.ry() < 0) {
                dy = 1.0;
            }
            this->chart()->scroll(0, dy);
        }
    }
}

ChartWidget::ChartWidget(QWidget *parent) : QWidget(parent)
{
    mainChart = new QChart();
    liveChart = new LiveChart(mainChart);
    liveChart->setMinimumSize(800, 600);
    m_series = new QLineSeries;
    mainChart->addSeries(m_series);
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 10000);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("t, ms");
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 1000);
    axisY->setTitleText("Signal, mV");
    mainChart->setAxisX(axisX, m_series);
    mainChart->setAxisY(axisY, m_series);
    mainChart->legend()->hide();
    mainChart->setTitle("ELO Chart data");
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

void ChartWidget::addDataToChart(qreal x, qreal y)
{
    m_series->append(x, y);
    qreal dx = x - prevPoint.rx();
    totalChartTime += dx;
    if(totalChartTime > 9000.0) {
        mainChart->scroll(dx / 20, 0);
    }
    prevPoint.setX(x);
    mainChart->update();
}

void ChartWidget::removeDataFromChart(qreal x, qreal y)
{
    m_series->remove(x, y);
    mainChart->update();
}

void ChartWidget::clearChart()
{
    m_series->clear();
    mainChart->update();
}
