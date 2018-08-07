#include "eloplotter.h"

ELOPlotter::ELOPlotter(QWidget *parent)
{
    this->setParent(parent);
    initPlotter();
    initColorList();
}

void ELOPlotter::addDataToPlot(int id, qreal x, qreal y)
{
    if(graphMap.contains(id)) {
        graphMap.value(id)->addData(x, y);
    }
    else {
        QCPGraph *newGraph = this->addGraph();
        graphMap.insert(id, newGraph);
        QColor plc = nextDefaultColor();
        newGraph->setPen(plc);
        newGraph->addData(x, y);
        emit plotColorChanged(id, plc);
    }
    updateRanges(x, y);
    this->replot();
}

void ELOPlotter::clearPlot()
{
    this->clearGraphs();
}

/**
 * @brief ELOPlotter::setPlotColor
 * @param id - unique identificator for plot
 * @param color - color for plot
 * Note: signal plotColorChanged not emitted in this case.
 */
void ELOPlotter::setPlotColor(int id, const QColor &color)
{
    if(graphMap.contains(id)) {
        graphMap.value(id)->setPen(color);
    }
}

/**
 * Private methods
 */

void ELOPlotter::initPlotter()
{
    this->setBackground(QBrush(QColor(0, 0, 0)));
    this->xAxis->setBasePen(QPen(Qt::white));
    this->xAxis->setTickPen(QPen(Qt::white));
    this->xAxis->grid()->setVisible(true);
    this->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
    this->xAxis->setTickLabelColor(Qt::white);
    this->xAxis->setLabelColor(Qt::white);
    this->yAxis->setBasePen(QPen(Qt::white));
    this->yAxis->setTickPen(QPen(Qt::white));
    this->yAxis->setSubTickPen(QPen(Qt::white));
    //this->yAxis->grid()->setSubGridVisible(true);
    this->yAxis->setTickLabelColor(Qt::white);
    this->yAxis->setLabelColor(Qt::white);
    this->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
    //this->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    this->xAxis->setRange(0.0, 10000.0);
    this->yAxis->setRange(-100.0, 2000.0);
    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables
                          | QCP::iSelectItems | QCP::iSelectAxes);
}

void ELOPlotter::initColorList()
{
    plotColors.append(Qt::red);
    plotColors.append(Qt::green);
    plotColors.append(Qt::blue);
    plotColors.append(Qt::cyan);
    plotColors.append(Qt::magenta);
    plotColors.append(Qt::yellow);
    plotColors.append(QColor(255, 128, 0));
    plotColors.append(Qt::white);
    plotColors.append(Qt::darkRed);
    plotColors.append(Qt::darkGreen);
    plotColors.append(Qt::darkBlue);
    plotColors.append(Qt::darkCyan);
    plotColors.append(Qt::darkMagenta);
    plotColors.append(Qt::darkYellow);
}

const QColor &ELOPlotter::nextDefaultColor() const
{
    int index = graphMap.size();
    if(index > 0) {
        index--;
    }
    if(index < plotColors.size()) {
        return plotColors.at(index);
    }
    else {
        return plotColors.at(0);
    }
}

void ELOPlotter::updateRanges(qreal x, qreal y)
{
    if(y > this->yAxis->range().upper) {
        this->yAxis->setRangeUpper(1.1 * y);
    }
    else if(y < this->yAxis->range().lower) {
        this->yAxis->setRangeLower(1.1 * y);
    }
    //times goes only forward
    if(x > this->xAxis->range().upper) {
        this->xAxis->moveRange(this->xAxis->range().size());
    }
}
