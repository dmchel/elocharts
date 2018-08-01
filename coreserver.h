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

    void setSoftVersion(const QString &str);

    ChartRecordModel *dataModel() const;
    CustomRecordDelegate *dataDelegate() const;

signals:
    void stopSerial();
    void connected();
    void disconnected();
    void sendConnectionStatus(bool flag);
    void connectionInfoChanged(const QString &str);
    void sendConsoleText(const QString &str);

    //shell signals
    void sendUartRxData(const QByteArray &data);
    void sendUartTxData(const QByteArray &data);

    void chartData(int id, qreal x, qreal y);

public slots:
    void openSerialPort(const QString &name);
    void closeSerialPort();
    //shell handlers
    void shellListenUart();
    void shellUartStatus();
    void shellSendUart(const QByteArray &data);
    void shellPrintPorts();
    void shellVersionRequest();

private slots:
    void onNewChartData(RawData value);
    void checkConnection();

private:
    void onOpenSerialPort();
    void onCloseSerialPort();
    void readSettings();
    void writeParamToSettings(const ParamDataItem &item);

private:
    ProtocolManager *protocol = Q_NULLPTR;
    SerialHandler *serialDevice = Q_NULLPTR;
    ProtocolData *dataVault = Q_NULLPTR;
    ChartRecordModel *chartModel = Q_NULLPTR;
    CustomRecordDelegate *chartDelegate = Q_NULLPTR;
    SettingsWizard *settings = Q_NULLPTR;

    QTimer *checkConnectionTimer = Q_NULLPTR;

    bool fListenUart = false;
    QString progVersion = "?????";
};

#endif // CORESERVER_H
