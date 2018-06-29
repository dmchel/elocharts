#include "protocoldata.h"

#include <QPointF>
#include <QTime>
#include <chrono>

ProtocolData::ProtocolData(QObject *parent) : QObject(parent)
{

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

void ProtocolData::setParamOnDevice(int id, quint32 val)
{
    SerialPacket pack;
    pack.cmd = ProtocolManager::SET_PARAM;
    pack.id = id;
    pack.data.word = val;
    emit generatePacket(pack);
}

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

void ProtocolData::saveParam(int id, quint32 val)
{
    //quint64 t = QTime::currentTime().msecsSinceStartOfDay();
    quint64 t = duration_cast<microseconds>(std::chrono::steady_clock::now());
    bool fExist = false;
    for(auto param : params) {
        if(param.id == id) {
            qint64 t = duration_cast<microseconds>(std::chrono::steady_clock::now());
            param.dots.append(QPointF(t - param.timestamp, val));
            param.timestamp = t;
            fExist = true;
            break;
        }
    }
    if(!fExist) {
        RawData data;
        data.id = id;
        data.timestamp = t;
        data.dots.append(QPointF(0, val));
        params.append(data);
    }
}
