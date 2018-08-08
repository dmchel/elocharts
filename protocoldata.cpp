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
    testTimer->start(0);
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
    int id = 0;
    quint32 val = 0;

    switch (pack.cmd) {
    case ProtocolManager::WRITE_PARAM:
        //параметр успешно записан
        if(pack.data.at(0) == -1) {
            emit writeErrorOccured();
        }
        break;
    case ProtocolManager::READ_PARAM:
        id = (pack.addr - BASE_MEMORY_ADDR) / 4 + 1;
        if((id > 0) && (pack.data.size() >= 4)) {
            val = 0;
            for(int i = 0; i < pack.data.size(); i++) {
                quint8 byte = pack.data.at(i);
                val |= byte << (i * 8);
                if((i + 1) % 4 == 0) {
                    saveParam(id, pack.timestamp, val);
                    val = 0;
                    id++;
                }
            }
        }
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
    pack.type = SerialPacket::HOST;
    pack.cmd = ProtocolManager::WRITE_PARAM;
    pack.num = 3;
    pack.addr = BASE_MEMORY_ADDR + id;
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
    pack.type = SerialPacket::HOST;
    pack.cmd = ProtocolManager::READ_PARAM;
    //pack.num = 3;
    pack.num = 12 - 1;
    pack.addr = 0x20000000 + (id - 1) * 4;
    emit generatePacket(pack);
}

/**
 * Private methods
 */

void ProtocolData::saveParam(int id, qint64 t, qint32 val)
{
    RawData newData;
    newData.id = id;
    newData.dot = QPointF(t - timestamp, val);
    emit dataArrived(newData);
}

void ProtocolData::onTestTimerTimeout()
{
    /*qint64 t = QDateTime::currentMSecsSinceEpoch();
    int x = 0;
    if(qSin(t - timestamp) < 0) {
        x = -1;
    }
    else if(qSin(t - timestamp) > 0) {
        x = 1;
    }
    saveParam(1, t, (qrand() % 1000));
    saveParam(2, t, 250 * x);
    saveParam(3, t, 125);*/
    static int param_id = 17;
    requesetParamFromDevice(param_id);
    //param_id++;
    //if(param_id == 23) {
    //    param_id = 17;
    //}
}
