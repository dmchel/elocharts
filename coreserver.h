#ifndef CORESERVER_H
#define CORESERVER_H

#include <QObject>
#include <QPointF>
#include <QHash>
#include <QAbstractProxyModel>

#include "protocoldata.h"
#include "recordModel/chartrecordmodel.h"
#include "recordModel/customrecorddelegate.h"

class ProtocolManager;
class SerialHandler;
class SettingsWizard;

/**
 * @brief The CoreServer class
 *  Core class for application business logic.
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

    void readSettings();

signals:
    void stopSerial();
    void connected();
    void disconnected();
    void sendConnectionStatus(bool flag);
    void connectionInfoChanged(const QString &str);
    void sendConsoleText(const QString &str);
    //to protocol data
    void writeParam(int id, quint32 val);
    void readParams(int start_id, int end_id);
    //shell signals
    void sendUartRxData(const QByteArray &data);
    void sendUartTxData(const QByteArray &data);
    //for graphics
    void chartData(int id, qreal x, qreal y);
    void sendChartColor(int id, const QColor &color);

public slots:
    void openSerialPort(const QString &name = "");
    void closeSerialPort();
    //gui data slots
    void addParamData(const QJsonObject &data);
    void onChartColorChange(int id, const QColor &color);
    void runUpdate();
    void pauseUpdate();
    void resetTimestamp();
    void saveSettings();
    //shell handlers
    void shellListenUart();
    void shellUartStatus();
    void shellSendUart(const QByteArray &data);
    void shellPrintPorts();
    void shellVersionRequest();

private slots:
    void onNewChartData(const RawData &value);
    void onModelDataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void checkConnection();
    void dataControl();

private:
    void onOpenSerialPort();
    void onCloseSerialPort();
    void writeParamToSettings(const ParamDataItem &item);
    void writeParamToSettings(const QJsonObject &obj);

private:
    ProtocolManager *protocol = Q_NULLPTR;
    SerialHandler *serialDevice = Q_NULLPTR;
    ProtocolData *dataVault = Q_NULLPTR;
    ChartRecordModel *chartModel = Q_NULLPTR;
    CustomRecordDelegate *chartDelegate = Q_NULLPTR;
    SettingsWizard *settings = Q_NULLPTR;
    QHash<int, int> paramTimeoutsHash;
    bool fRunUpdate = true;

    QTimer *checkConnectionTimer = Q_NULLPTR;
    QTimer *requestTimer = Q_NULLPTR;

    bool fListenUart = false;
    QString progVersion = "?????";

    const int REQUEST_DATA_PERIOD_MS = 10;
};

#endif // CORESERVER_H
