#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtWidgets/QVBoxLayout>
#include <QtCharts/QValueAxis>

#include "chartwidget.h"

ChartWidget::ChartWidget(QWidget *parent) : QWidget(parent)
{
    m_chart = new QChart;
    QChartView *chartView = new QChartView(m_chart);
    chartView->setMinimumSize(800, 600);
    m_series = new QLineSeries;
    m_chart->addSeries(m_series);
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 100000);
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

void ChartWidget::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}

void ChartWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        //this->scroll(numPixels.rx(), numPixels.ry());
        m_chart->scroll(numPixels.ry(), numPixels.ry());
    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees;
        //this->scroll(numSteps.rx(), numSteps.ry());
        m_chart->scroll(numSteps.ry(), numSteps.ry());
    }

    event->accept();
}

void ChartWidget::addDataToChart(qreal x, qreal y)
{
    m_series->append(x, y);
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
