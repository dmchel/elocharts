#include "protocolmanager.h"

#include <QByteArray>
#include <QTimer>

SerialPacket::SerialPacket(quint8 _id, quint32 _data)
{
    id = _id;
    data.word = _data;
}

void SerialPacket::clear()
{
    cmd = 0;
    id = 0;
    data.word = 0;
    //fExtended = false;
}

ProtocolManager::ProtocolManager(QObject *parent) : QObject(parent)
{
    rxBuffer.clear();
    rxState = ReceiverState::WAIT_START_FRAME;
    currPack.clear();
    qRegisterMetaType<SerialPacket>("SerialPacket");
    qRegisterMetaType<CommunicationStatistic>("CommunicationStatistic");
    checkConnectPeriod = 5000;
    memset(&statistic, 0, sizeof(statistic));

    stateTimer = new QTimer(this);
    stateTimer->setTimerType(Qt::PreciseTimer);
    stateTimer->setSingleShot(true);
    connect(stateTimer, &QTimer::timeout, this, &ProtocolManager::transferTimeout);

    onlineTimer = new QTimer(this);
    onlineTimer->setTimerType(Qt::PreciseTimer);
    onlineTimer->setSingleShot(true);
    connect(onlineTimer, &QTimer::timeout, this, &ProtocolManager::onlineTimeout);
}

ProtocolManager::CommunicationStatistic ProtocolManager::commStatus() const
{
    return statistic;
}

/**
 * @brief ProtocolManager::connectionLostTimeout
 * @return Таймаут определения потери связи по каналу
 */
int ProtocolManager::connectionLostTimeout() const
{
    return checkConnectPeriod;
}

/**
 * @brief ProtocolManager::setConnectionLostTimeout
 *  Установить таймаут определения потери связи по каналу
 * @param value, ms
 */
void ProtocolManager::setConnectionLostTimeout(int value)
{
    checkConnectPeriod = value;
}

void ProtocolManager::stop()
{
    emit finished();
}

/**
 * @brief ProtocolManager::putData
 * Новые данные для обработки
 * @param data
 */
void ProtocolManager::receiveData(const QByteArray &data)
{
    statistic.rxBytes += data.size();
    rxBuffer.append(data);
    stateTimer->start(1000);
    dataHandler();
    //emit sendCommStatistic(statistic);
}

/**
 * @brief ProtocolManager::sendPacket
 * Отправка пакета данных в последовательный порт
 * @param pack пакет для отправки
 * ret массив данных для отправки (это для тестов)
 */
QByteArray ProtocolManager::sendPacket(const SerialPacket &pack)
{
    QByteArray dataToSend;
    dataToSend.append(FS_START);
    dataToSend.append(pack.cmd);
    dataToSend.append(pack.id);
    if(pack.cmd == SET_PARAM) {
        for(int i = 0; i < DATA_SIZE; i++) {
            dataToSend.append(pack.data.bytes[i]);
        }
        dataToSend.append(computeCRC(pack));
    }
    emit transmitData(dataToSend);
    statistic.txBytes += dataToSend.size();
    statistic.txPackets++;
    //emit sendCommStatistic(statistic);
    return dataToSend;
}

/**
 * Private methods
 */

/**
 * @brief ProtocolManager::transferTimeout
 *  Receive timeout happens
 */
void ProtocolManager::transferTimeout()
{
    if(rxState != ReceiverState::WAIT_START_FRAME) {
        rxState = ReceiverState::FRAME_ERROR;
        statistic.timeoutErrors++;
        statistic.overallErrors++;
        //emit sendCommStatistic(statistic);
    }
}

/**
 * @brief ProtocolManager::onlineTimeout
 *  Lost connection timeout happens
 */
void ProtocolManager::onlineTimeout()
{
    statistic.fConnected = false;
    //emit sendCommStatistic(statistic);
}

/**
 * @brief ProtocolManager::dataHandler
 *  Byte-to-byte data handler
 */
void ProtocolManager::dataHandler()
{
    while(!rxBuffer.isEmpty()) {
        quint8 c = rxBuffer[0];
        switch (rxState) {
        case ReceiverState::WAIT_START_FRAME:
            rxState = checkStartMark(c);
            break;
        case ReceiverState::WAIT_CMD:
            rxState = checkCmd(c);
            break;
        case ReceiverState::WAIT_ID:
            rxState = checkId(c);
            break;
        case ReceiverState::DATA_FLOW:
            rxState = checkData(c);
            break;
        case ReceiverState::WAIT_CRC:
            rxState = checkCrc(c);
            break;
        case ReceiverState::FRAME_ERROR:
            currPack.clear();
            rxState = checkStartMark(c);
            break;
        default:
            break;
        }
        rxBuffer.remove(0, 1);
    }
}

ProtocolManager::ReceiverState ProtocolManager::checkStartMark(quint8 byte)
{
    if(byte == FS_START) {
        return ReceiverState::WAIT_CMD;
    }
    return ReceiverState::FRAME_ERROR;
}

ProtocolManager::ReceiverState ProtocolManager::checkCmd(quint8 byte)
{
    if((byte == SEND_PARAM) || (byte == CONFIRM_PARAM)) {
        currPack.cmd = byte;
        return ReceiverState::WAIT_ID;
    }
    else {
        onFormatError();
        return ReceiverState::FRAME_ERROR;
    }
}

ProtocolManager::ReceiverState ProtocolManager::checkId(quint8 byte)
{
    currPack.id = byte;
    dataCounter = 0;
    return ReceiverState::DATA_FLOW;
}

ProtocolManager::ReceiverState ProtocolManager::checkData(quint8 byte)
{
    ReceiverState state = ReceiverState::DATA_FLOW;
    currPack.data.bytes[dataCounter] = byte;
    dataCounter++;
    if(dataCounter == DATA_SIZE) {
        //wait crc for extended packs
        if(currPack.cmd == CONFIRM_PARAM) {
            state = ReceiverState::WAIT_CRC;
        }
        //standard pack without crc - it's done
        else {
            emit packRecieved(currPack);
            state = ReceiverState::WAIT_START_FRAME;
            onValidPackReceive();
        }
    }
    return state;
}

ProtocolManager::ReceiverState ProtocolManager::checkCrc(quint8 byte)
{
    ReceiverState res = ReceiverState::FRAME_ERROR;
    if(computeCRC(currPack) == byte) {
        emit packRecieved(currPack);
        res = ReceiverState::WAIT_START_FRAME;
        onValidPackReceive();
    }
    return res;
}

quint8 ProtocolManager::computeCRC(const SerialPacket &pack)
{
    quint8 crc = 0;
    QByteArray crcData;
    crcData.append(FS_START);
    crcData.append(pack.cmd);
    crcData.append(pack.id);
    for(int i = 0; i < 4; i++) {
        crcData.append(pack.data.bytes[i]);
    }
    crc = crc8_xxx(reinterpret_cast<quint8*>(crcData.data()), crcData.size());
    return crc;
}

/**
 * @brief ProtocolManager::onValidPackReceive
 *  Получен валидный пакет
 */
void ProtocolManager::onValidPackReceive()
{
    statistic.fConnected = true;
    statistic.rxPackets++;
    //emit sendCommStatistic(statistic);
    onlineTimer->start(checkConnectPeriod);
}

/**
 * @brief ProtocolManager::onCrcError
 *  Ошибка контрольной суммы
 */
void ProtocolManager::onCrcError()
{
    statistic.crcErrors++;
    statistic.overallErrors++;
    //emit sendCommStatistic(statistic);
}

/**
 * @brief ProtocolManager::onFormatError
 *  Ошибка формата пакета
 */
void ProtocolManager::onFormatError()
{
    statistic.formatErrors++;
    statistic.overallErrors++;
    //emit sendCommStatistic(statistic);
}

quint8 ProtocolManager::crc8_xxx(quint8 *pcBlock, quint8 len)
{
    quint8 crc = POLY_CONST;

    while(len--) {
        crc = crc8Table[crc ^ *pcBlock++];
    }
    return crc;
}
