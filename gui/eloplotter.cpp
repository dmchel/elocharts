#include "eloplotter.h"

ELOPlotter::ELOPlotter(QWidget *parent)
{
    this->setParent(parent);
    initPlotter();
    initColorList();

    connect(this, &ELOPlotter::mousePress, this, &ELOPlotter::onMousePressed);
    connect(this, &ELOPlotter::selectionChangedByUser, this, &ELOPlotter::onSelectionChanged);
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
    this->replot(QCustomPlot::rpQueuedReplot);
}

void ELOPlotter::clearPlot()
{
    this->clearGraphs();
    fVerticalAutoScroll = true;
    maxRangeY = 0.0;
    this->replot();
}

void ELOPlotter::fitInPlots()
{
    fVerticalAutoScroll = true;
    updateRanges(0, maxRangeY);
    this->replot();
}

void ELOPlotter::pause()
{

}

void ELOPlotter::run()
{
    fVerticalAutoScroll = true;
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

void ELOPlotter::onMousePressed(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        fVerticalAutoScroll = false;
    }
}

void ELOPlotter::onSelectionChanged()
{
    int zoomOrientation = 0;
    for(auto axis : this->selectedAxes()) {
        if(axis->axisType() == QCPAxis::atLeft) {
            zoomOrientation |= Qt::Vertical;
        }
        else if(axis->axisType() == QCPAxis::atBottom) {
            zoomOrientation |= Qt::Horizontal;
        }
    }
    if(zoomOrientation != 0) {
        this->axisRect()->setRangeZoom((Qt::Orientation)zoomOrientation);
    }
    else {
        this->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    }
}

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
    //default range X = 10 sec
    this->xAxis->setRange(0.0, 10000.0);
    //default range Y
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
    if(fVerticalAutoScroll) {
        if(y > this->yAxis->range().upper) {
            maxRangeY = y;
            this->yAxis->setRangeUpper(1.1 * y);
        }
        else if(y < this->yAxis->range().lower) {
            this->yAxis->setRangeLower(1.1 * y);
        }
    }
    //times goes only forward
    if(x > this->xAxis->range().upper) {
        this->xAxis->moveRange(this->xAxis->range().size() / 2);
    }
}
