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
    axisX->setRange(0, 20000);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("t, ms");
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 1000);
    axisY->setTitleText("Signal, mV");
    m_chart->setAxisX(axisX, m_series);
    m_chart->setAxisY(axisY, m_series);
    m_chart->legend()->hide();
    m_chart->setTitle("ELO Chart data");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(chartView);
    setLayout(mainLayout);
}

ChartWidget::~ChartWidget()
{
    delete m_chart;
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
