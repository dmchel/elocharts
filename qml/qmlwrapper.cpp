#include "qmlwrapper.h"

QMLWrapper::QMLWrapper(QObject *parent) : QObject(parent)
{
    initConnections();
}

void QMLWrapper::addDataToChart(qreal x, qreal y)
{
    emit addPoint(x, y);
}

void QMLWrapper::removeDataFromChart(qreal x, qreal y)
{
    emit removePoint(x, y);
}

void QMLWrapper::clearChart()
{
    emit clear();
}

/**
 * Private methods
 */
void QMLWrapper::initConnections()
{
    connect(this, SIGNAL(addPoint(QVariant, QVariant)), this->parent(), SLOT(addDataToLineSeries(QVariant, QVariant)));
    connect(this, SIGNAL(removePoint(QVariant,QVariant)), this->parent(), SLOT(removeDataFromLineSeries(QVariant, QVariant)));
    connect(this, SIGNAL(clear()), this->parent(), SLOT(clearLineSeries()));
}
