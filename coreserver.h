#ifndef CORESERVER_H
#define CORESERVER_H

#include <QObject>
#include <QPointF>

#include "protocoldata.h"

class ProtocolManager;
class SerialHandler;
class ProtocolData;
class ChartRecordModel;
class SettingsWizard;

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

    ChartRecordModel *dataModel() const;

signals:
    void stopSerial();
    void connected();
    void disconnected();

    void chartData(qreal x, qreal y);

public slots:
    void openSerialPort(const QString &name);
    void closeSerialPort();

private slots:
    void onNewChartData(RawData value);

private:
    void onOpenSerialPort();
    void onCloseSerialPort();
    void readSettings();
    void writeParamToSettings(int id);

private:
    ProtocolManager *protocol = Q_NULLPTR;
    SerialHandler *serialDevice = Q_NULLPTR;
    ProtocolData *dataVault = Q_NULLPTR;
    ChartRecordModel *chartModel = Q_NULLPTR;
    SettingsWizard *settings = Q_NULLPTR;
};

#endif // CORESERVER_H
