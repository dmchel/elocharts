#ifndef CORESERVER_H
#define CORESERVER_H

#include <QObject>
#include <QPointF>

#include "protocoldata.h"
#include "recordModel/chartrecordmodel.h"
#include "recordModel/customrecorddelegate.h"

class ProtocolManager;
class SerialHandler;
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
    CustomRecordDelegate *dataDelegate() const;

signals:
    void stopSerial();
    void connected();
    void disconnected();
    void sendConnectionStatus(bool flag);
    void connectionInfoChanged(const QString &str);

    void chartData(int id, qreal x, qreal y);

public slots:
    void openSerialPort(const QString &name);
    void closeSerialPort();

private slots:
    void onNewChartData(RawData value);
    void checkConnection();

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
    CustomRecordDelegate *chartDelegate = Q_NULLPTR;
    SettingsWizard *settings = Q_NULLPTR;

    QTimer *checkConnectionTimer = Q_NULLPTR;
};

#endif // CORESERVER_H
