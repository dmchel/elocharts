#ifndef PROTOCOLDATA_H
#define PROTOCOLDATA_H

#include <QObject>
#include <QVector>

#include "protocolmanager.h"

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

public slots:
    void packetHandler(const SerialPacket &pack);
    void setParamOnDevice(int id, quint32 val);
    void requesetParamFromDevice(int id);

private:
    void saveParam(int id, quint32 val);

private:
    QVector<RawData> params;

};

#endif // PROTOCOLDATA_H
