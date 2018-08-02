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
        newGraph->setPen(nextDefaultColor());
        newGraph->addData(x, y);
        //this->graph(id - 1)->addData(x, y);
    }
    this->replot();
}

void ELOPlotter::clearPlot()
{
    this->clearGraphs();
}

void ELOPlotter::setPlotColor(int id, QColor color)
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
    this->setBackground(QBrush(Qt::gray));
    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void ELOPlotter::initColorList()
{
    plotColors.append(Qt::red);
    plotColors.append(Qt::green);
    plotColors.append(Qt::blue);
    plotColors.append(Qt::cyan);
    plotColors.append(Qt::magenta);
    plotColors.append(Qt::yellow);
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
