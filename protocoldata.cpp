#include "protocoldata.h"

#include <Qtimer>
#include <QTime>
#include <chrono>

ProtocolData::ProtocolData(QObject *parent) : QObject(parent)
{
    testTimer = new QTimer(this);
    testTimer->setTimerType(Qt::PreciseTimer);
    connect(testTimer, &QTimer::timeout, this, &ProtocolData::onTestTimerTimeout);
    testTimer->start(10);
    qsrand(42);
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
    quint64 t = QTime::currentTime().msecsSinceStartOfDay();
    QPointF point = QPointF(0, val);
    //quint64 t = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now());
    bool fExist = false;
    for(auto param : params) {
        if(param.id == id) {
            point = QPointF(t - param.timestamp, val);
            param.dots.append(point);
            param.timestamp = t;
            fExist = true;
            break;
        }
    }
    if(!fExist) {
        RawData data;
        data.id = id;
        data.timestamp = t;
        data.dots.append(point);
        params.append(data);
    }
    emit dataArrived(point);
}

void ProtocolData::onTestTimerTimeout()
{
    saveParam(1, (qrand() % 1000));
}
