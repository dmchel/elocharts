#ifndef PROTOCOLDATA_H
#define PROTOCOLDATA_H

#include <QObject>
#include "protocolmanager.h"

class ProtocolData : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolData(QObject *parent = nullptr);
    ~ProtocolData();

signals:

public slots:
    void packetHandler(const SerialPacket &pack);
};

#endif // PROTOCOLDATA_H
