#ifndef PROTOCOLDATA_H
#define PROTOCOLDATA_H

#include <QObject>
#include <QVector>
#include <QPointF>

#include "protocolmanager.h"

class QTimer;

struct RawData {
    int id;
    QPointF dot;
};

/**
 * @brief The ProtocolData class
 *  Объект для обработки данных протокола обмена с устройством
 */
class ProtocolData : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolData(QObject *parent = nullptr);

signals:
    void generatePacket(const SerialPacket &pack);
    void dataArrived(RawData value);

public slots:
    void resetTimestamp();
    void packetHandler(const SerialPacket &pack);
    void setParamOnDevice(int id, quint32 val);
    void requesetParamFromDevice(int id);

private slots:
    void onTestTimerTimeout();

private:
    void saveParam(int id, qint32 val);

private:
    qint64 timestamp = 0;                               //начальная отметка времени (для расчета x-координаты точки)
    QTimer *testTimer = Q_NULLPTR;

};

#endif // PROTOCOLDATA_H
