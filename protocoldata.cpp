#include "protocoldata.h"

#include <QTimer>
#include <QDateTime>
//#include <chrono>
#include <math.h>
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
    case ProtocolManager::SEND_PARAM:
        saveParam(pack.id, pack.data.word);
        break;
    case ProtocolManager::CONFIRM_PARAM:
        //TODO
        break;
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
    pack.cmd = ProtocolManager::SET_PARAM;
    pack.id = id;
    pack.data.word = val;
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
    pack.cmd = ProtocolManager::GET_PARAM;
    pack.id = id;
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
    //quint64 t = QDateTime::currentMSecsSinceEpoch();
    //saveParam(1, 50 * sin(0.05 * t));
    saveParam(1, (qrand() % 1000));
}
