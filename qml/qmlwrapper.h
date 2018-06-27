#ifndef QMLQMLWRAPPER_H
#define QMLQMLWRAPPER_H

#include <QObject>

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

public slots:
};

#endif // QMLQMLWRAPPER_H
