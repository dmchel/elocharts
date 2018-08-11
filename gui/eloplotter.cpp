#include "eloplotter.h"

ELOPlotter::ELOPlotter(QWidget *parent)
{
    this->setParent(parent);
    initPlotter();
    initColorList();

    connect(this, &ELOPlotter::mousePress, this, &ELOPlotter::onMousePressed);
    connect(this, &ELOPlotter::selectionChangedByUser, this, &ELOPlotter::onSelectionChanged);

    updateTimer = new QTimer(this);
    updateTimer->setTimerType(Qt::PreciseTimer);
    updateTimer->setInterval(replotIntervalMs);
    connect(updateTimer, &QTimer::timeout, this, &ELOPlotter::onUpdateTimeout);
}

void ELOPlotter::setUpdatingPeriod(int msec)
{
    replotIntervalMs = msec;
    updateTimer->start(replotIntervalMs);
}

int ELOPlotter::updatingInterval() const
{
    return replotIntervalMs;
}

void ELOPlotter::start(int msec)
{
    replotIntervalMs = msec;
    updateTimer->start(msec);
}

void ELOPlotter::addDataToPlot(int id, qreal x, qreal y)
{
    if(!fRunning) {
        return;
    }
    if(graphMap.contains(id)) {
        graphMap.value(id)->addData(x, y);
    }
    else {
        QCPGraph *newGraph = this->addGraph();
        graphMap.insert(id, newGraph);
        QColor plc = nextDefaultColor();
        if(colorMap.contains(id)) {
            plc = colorMap.value(id);
        }
        QPen pen;
        pen.setColor(plc);
        pen.setWidth(1);
        newGraph->setPen(pen);
        newGraph->addData(x, y);
        emit plotColorChanged(id, plc);
    }
    updateRanges(x, y);
    //now replot control by timer
    //this->replot(QCustomPlot::rpQueuedReplot);
}

void ELOPlotter::clearPlot()
{
    graphMap.clear();
    this->clearGraphs();
    timeLine = this->addGraph();
    timeLine->setPen(QColor(Qt::white));
    fVerticalAutoScroll = true;
    maxRangeY = 0.0;
    minRangeY = 0.0;
    setDefaultRanges();
    this->replot();
}

void ELOPlotter::fitInPlots()
{
    fVerticalAutoScroll = true;
    this->yAxis->setRange(1.1 * minRangeY, 1.1 * maxRangeY);
    this->replot();
}

void ELOPlotter::pause()
{
    fRunning = false;
}

void ELOPlotter::run()
{
    fVerticalAutoScroll = true;
    fRunning = true;
}

/**
 * @brief ELOPlotter::setPlotColor
 * @param id - unique identificator for plot
 * @param color - color for plot
 * Note: signal plotColorChanged not emitted in this case.
 */
void ELOPlotter::setPlotColor(int id, const QColor &color)
{
    colorMap.insert(id, color);
    if(graphMap.contains(id)) {
        graphMap.value(id)->setPen(color);
    }
}

/**
 * Private methods
 */

void ELOPlotter::onUpdateTimeout()
{
    this->replot();
}

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
    setDefaultRanges();

    timeLine = this->addGraph();
    timeLine->setPen(QColor(Qt::white));
    tlKeys.append(0.0);
    tlKeys.append(0.0);
    tlData.append(yAxis->range().upper);
    tlData.append(yAxis->range().lower);

    this->setNotAntialiasedElements(QCP::aeAll);
    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    this->xAxis->setTickLabelFont(font);
    this->yAxis->setTickLabelFont(font);
    this->legend->setFont(font);

    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
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

void ELOPlotter::setDefaultRanges()
{
    //default range X = 10 sec
    this->xAxis->setRange(0.0, 10000.0);
    //default range Y
    this->yAxis->setRange(-100.0, 2000.0);
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
            this->yAxis->setRangeUpper(1.1 * y);
        }
        else if(y < this->yAxis->range().lower) {
            this->yAxis->setRangeLower(1.1 * y);
        }
    }
    if(y > maxRangeY) {
        maxRangeY = y;
    }
    else if(y < minRangeY) {
        minRangeY = y;
    }
    //times goes only forward
    if(x > this->xAxis->range().upper) {
        this->xAxis->moveRange(this->xAxis->range().size() / 2);
    }
    tlKeys[0] = x;
    tlKeys[1] = x;
    tlData[0] = this->yAxis->range().upper;
    tlData[1] = this->yAxis->range().lower;
    timeLine->setData(tlKeys, tlData);
}
