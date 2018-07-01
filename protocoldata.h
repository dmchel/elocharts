#ifndef PROTOCOLDATA_H
#define PROTOCOLDATA_H

#include <QObject>
#include <QVector>
#include <QPointF>

#include "protocolmanager.h"

class QTimer;

struct RawData {
    int id;
    qint64 timestamp;
    QVector<QPointF> dots;
};

class ProtocolData : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolData(QObject *parent = nullptr);

signals:
    void generatePacket(const SerialPacket &pack);
    void dataArrived(QPointF point);

public slots:
    void packetHandler(const SerialPacket &pack);
    void setParamOnDevice(int id, quint32 val);
    void requesetParamFromDevice(int id);

private slots:
    void onTestTimerTimeout();

private:
    void saveParam(int id, quint32 val);

private:
    QVector<RawData> params;
    QTimer *testTimer;

};

#endif // PROTOCOLDATA_H
