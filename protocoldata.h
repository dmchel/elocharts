#ifndef PROTOCOLDATA_H
#define PROTOCOLDATA_H

#include <QObject>
#include <QVector>
#include <QPointF>

#include "protocolmanager.h"

class QTimer;

struct RawData {
    int id;
    QPoint dot;
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
    void writeErrorOccured();

public slots:
    void resetTimestamp();
    void packetHandler(const SerialPacket &pack);
    void setParamOnDevice(int id, quint32 val);
    void requesetParamsFromDevice(int start_id, int end_id = 0);

private slots:
    void onTestTimerTimeout();

private:
    void saveParam(int id, qint64 t, qint32 val);

private:
    qint64 timestamp = 0;                               //начальная отметка времени (для расчета x-координаты точки)
    QTimer *testTimer = Q_NULLPTR;

    const int BASE_MEMORY_ADDR = 0x20000000;

};

#endif // PROTOCOLDATA_H
