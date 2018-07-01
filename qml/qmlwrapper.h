#ifndef QMLQMLWRAPPER_H
#define QMLQMLWRAPPER_H

#include <QObject>
#include <QVariant>

/**
 * @brief The QMLWrapper class
 *  Simple wrapper for qml-part of application.
 */
class QMLWrapper : public QObject
{
    Q_OBJECT
public:
    explicit QMLWrapper(QObject *parent = nullptr);

signals:
    void addPoint(QVariant x, QVariant y);
    void removePoint(QVariant x, QVariant y);
    void clear();

public slots:
    void addDataToChart(qreal x, qreal y);
    void removeDataFromChart(qreal x, qreal y);
    void clearChart();

private:
    void initConnections();

};

#endif // QMLQMLWRAPPER_H
