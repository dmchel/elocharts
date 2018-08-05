#include "protocoldata.h"

#include <QTimer>
#include <QDateTime>
//#include <chrono>
#include <qmath.h>
#include <QDebug>

ProtocolData::ProtocolData(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<RawData>("RawData");
    testTimer = new QTimer(this);
    testTimer->setTimerType(Qt::PreciseTimer);
    connect(testTimer, &QTimer::timeout, this, &ProtocolData::onTestTimerTimeout);
    testTimer->start(100);
    qsrand(42);
    timestamp = QDateTime::currentMSecsSinceEpoch();
}

void ProtocolData::resetTimestamp()
{
    timestamp = QDateTime::currentMSecsSinceEpoch();
}

/**
 * @brief ProtocolData::packetHandler
 *  Обработка пакетов от устройства
 * @param pack
 */
void ProtocolData::packetHandler(const SerialPacket &pack)
{
    switch (pack.cmd) {
    //case ProtocolManager::SEND_PARAM:
    //    saveParam(pack.id, pack.data.word);
    //    break;
    //case ProtocolManager::CONFIRM_PARAM:
        //TODO
     //   break;
    default:
        break;
    }
}

/**
 * @brief ProtocolData::setParamOnDevice
 *  Установить значения параметра на устройстве
 * @param id
 * @param val
 */
void ProtocolData::setParamOnDevice(int id, quint32 val)
{
    SerialPacket pack;
    pack.type = SerialPacket::HOST;
    pack.cmd = ProtocolManager::WRITE_PARAM;
    pack.num = 4;
    pack.addr = 0x20000000 + id;
    pack.data.append(val & 0xFF);
    pack.data.append((val >> 8) & 0xFF);
    pack.data.append((val >> 16) & 0xFF);
    pack.data.append((val >> 24) & 0xFF);
    emit generatePacket(pack);
}

/**
 * @brief ProtocolData::requesetParamFromDevice
 *  Запросить значение параметра с устройства
 * @param id
 */
void ProtocolData::requesetParamFromDevice(int id)
{
    SerialPacket pack;
    pack.cmd = ProtocolManager::READ_PARAM;
    pack.num = 4;
    pack.addr = 0x20000000 + id;
    emit generatePacket(pack);
}

/**
 * Private methods
 */

void ProtocolData::saveParam(int id, qint32 val)
{
    quint64 t = QDateTime::currentMSecsSinceEpoch();
    RawData newData;
    newData.id = id;
    newData.dot = QPointF(t - timestamp, val);
    emit dataArrived(newData);
}

void ProtocolData::onTestTimerTimeout()
{
    quint64 t = QDateTime::currentMSecsSinceEpoch();
    int x = 0;
    if(qSin(t - timestamp) < 0) {
        x = -1;
    }
    else if(qSin(t - timestamp) > 0) {
        x = 1;
    }
    saveParam(1, (qrand() % 1000));
    saveParam(2, 250 * x);
    saveParam(3, 125);
}
