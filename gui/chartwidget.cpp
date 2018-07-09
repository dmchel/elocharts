#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtWidgets/QVBoxLayout>
#include <QtCharts/QValueAxis>
#include <QDebug>

#include "chartwidget.h"

ChartWidget::ChartWidget(QWidget *parent) : QWidget(parent)
{
    m_chart = new QChart;
    QChartView *chartView = new QChartView(m_chart);
    chartView->setMinimumSize(800, 600);
    m_series = new QLineSeries;
    m_chart->addSeries(m_series);
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 10000);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("t, ms");
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 1000);
    axisY->setTitleText("Signal, mV");
    m_chart->setAxisX(axisX, m_series);
    m_chart->setAxisY(axisY, m_series);
    m_chart->legend()->hide();
    m_chart->setTitle("ELO Chart data");
    //m_chart->setBackgroundBrush(QColor(64, 64, 64));
    //m_chart->setTitleBrush(QColor(Qt::blue));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(chartView);
    setLayout(mainLayout);
}

ChartWidget::~ChartWidget()
{
    delete m_chart;
}

void ChartWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        fLeftButton = true;
    }
    //event->accept();
}

void ChartWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        fLeftButton = false;
    }
    //event->accept();
}

void ChartWidget::mouseMoveEvent(QMouseEvent *event)
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
            m_chart->scroll(dx, dy);
        }
        //event->accept();
    }
}

void ChartWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        //m_chart->scroll(numPixels.ry(), numPixels.ry());
        if(numPixels.ry() > 0) {
            m_chart->zoomIn();
        }
        else if(numPixels.ry() < 0) {
            m_chart->zoomOut();
        }
    } else if (!numDegrees.isNull()) {
        //QPoint numSteps = numDegrees;
        //m_chart->scroll(numSteps.ry(), numSteps.ry());
        if(numDegrees.ry() > 0) {
            m_chart->zoomIn();
        }
        else if(numDegrees.ry() < 0) {
            m_chart->zoomOut();
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
        m_chart->scroll(dx / 20, 0);
    }
    prevPoint.setX(x);
    m_chart->update();
}

void ChartWidget::removeDataFromChart(qreal x, qreal y)
{
    m_series->remove(x, y);
    m_chart->update();
}

void ChartWidget::clearChart()
{
    m_series->clear();
    m_chart->update();
}
