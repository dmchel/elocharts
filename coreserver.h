#ifndef CORESERVER_H
#define CORESERVER_H

#include <QObject>
#include <QPointF>

class ProtocolManager;
class SerialHandler;
class ProtocolData;

/**
 * @brief The CoreServer class
 *  Core class for application buisness logic.
 */
class CoreServer : public QObject
{
    Q_OBJECT
public:
    explicit CoreServer(QObject *parent = nullptr);
    ~CoreServer();

signals:
    void stopSerial();
    void connected();
    void disconnected();

    void chartData(qreal x, qreal y);

public slots:
    void openSerialPort(const QString &name);
    void closeSerialPort();

private slots:
    void onNewChartData(QPointF point);

private:
    void onOpenSerialPort();
    void onCloseSerialPort();

private:
    ProtocolManager *protocol;
    SerialHandler *serialDevice;
    ProtocolData *dataVault;
};

#endif // CORESERVER_H
