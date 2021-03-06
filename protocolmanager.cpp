#include "protocolmanager.h"

#include <QByteArray>
#include <QDateTime>
#include <QTimer>

SerialPacket::SerialPacket(quint8 _cmd, quint32 _addr, quint8 _num, PACK_TYPE _type)
{
    cmd = _cmd;
    addr = _addr;
    type = _type;
    num = _num;
    timestamp = QDateTime::currentMSecsSinceEpoch();
}

void SerialPacket::clear()
{
    cmd = 0;
    addr = 0;
    num = 0;
    data.clear();
}

ProtocolManager::ProtocolManager(QObject *parent) : QObject(parent)
{
    rxBuffer.clear();
    rxState = ReceiverState::FREE;
    currRxPack.clear();
    currRxPack.type = SerialPacket::DEVICE;
    currTxPack.clear();
    currTxPack.type = SerialPacket::HOST;
    qRegisterMetaType<SerialPacket>("SerialPacket");
    qRegisterMetaType<CommunicationStatistic>("CommunicationStatistic");
    checkConnectPeriod = 5000;
    memset(&statistic, 0, sizeof(statistic));
    memset(&prevStatistic, 0, sizeof(prevStatistic));

    stateTimer = new QTimer(this);
    stateTimer->setTimerType(Qt::PreciseTimer);
    stateTimer->setSingleShot(true);
    connect(stateTimer, &QTimer::timeout, this, &ProtocolManager::transferTimeout);

    onlineTimer = new QTimer(this);
    //onlineTimer->setTimerType(Qt::PreciseTimer);
    onlineTimer->setSingleShot(true);
    connect(onlineTimer, &QTimer::timeout, this, &ProtocolManager::onlineTimeout);

    rateTimer = new QTimer(this);
    rateTimer->setTimerType(Qt::PreciseTimer);
    rateTimer->start(1000);
    connect(rateTimer, &QTimer::timeout, this, &ProtocolManager::updateRates);
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
 * ret none
 */
void ProtocolManager::sendPacket(const SerialPacket &pack)
{
    if(txQueue.size() == MAX_TX_QUEUE_SIZE) {
        txQueue.dequeue();
    }
    txQueue.enqueue(pack);
    processQueue();
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
    if(rxState != ReceiverState::FREE) {
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
 * @brief updateRates
 *  Calculate connection speed
 */
void ProtocolManager::updateRates()
{
    if(statistic.fConnected) {
        statistic.txSpeed = statistic.txBytes - prevStatistic.txBytes;
        statistic.rxSpeed = statistic.rxBytes - prevStatistic.rxBytes;
    }
    else {
        statistic.txSpeed = 0;
        statistic.rxSpeed = 0;
    }
    prevStatistic = statistic;
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
        case ReceiverState::FREE:
            //unexpected data
            break;
        case ReceiverState::DATA_FLOW:
            rxState = checkData(c);
            break;
        case ReceiverState::WAIT_CRC:
            rxState = checkCrc(c);
            processQueue();
            break;
        case ReceiverState::FRAME_ERROR:
            currRxPack.clear();
            break;
        default:
            break;
        }
        rxBuffer.remove(0, 1);
    }
}

/**
 * @brief ProtocolManager::processQueue
 *  Deal with TX queue
 */
void ProtocolManager::processQueue()
{
    if(!txQueue.isEmpty() && ((rxState == ReceiverState::FREE) || (rxState == ReceiverState::FRAME_ERROR))) {
        currTxPack = txQueue.dequeue();
        packTransmitter(currTxPack);
        currRxPack.clear();
        rxState = ReceiverState::DATA_FLOW;
    }
}

void ProtocolManager::packTransmitter(const SerialPacket &pack)
{
    QByteArray dataToSend;
    quint8 stuffByte;
    dataToSend.append(STX);
    dataToSend.append(pack.cmd);
    //адрес нужно проверять на стафинг
    for(int i = 0; i < 4; i++) {
        quint8 addrByte = (pack.addr >> (8 * i)) & 0xFF;
        stuffByte = tryToStuffByte(addrByte);
        if(stuffByte != addrByte) {
            dataToSend.append(DLE);
        }
        dataToSend.append(addrByte);
    }
    //размер данных и сами данные также стафятся
    stuffByte = tryToStuffByte(pack.num);
    if(stuffByte != pack.num) {
        dataToSend.append(DLE);
    }
    dataToSend.append(stuffByte);
    if(pack.cmd == WRITE_PARAM) {
        if(pack.num) {
            for(int i = 0; (i < pack.data.size()) && (i < pack.num); i++) {
                stuffByte = tryToStuffByte(pack.data[i]);
                if(stuffByte != pack.data[i]) {
                    dataToSend.append(DLE);
                }
                dataToSend.append(stuffByte);
            }
        }
    }
    quint8 crc = computeCRC(pack);
    stuffByte = tryToStuffByte(crc);
    if(stuffByte != crc) {
        dataToSend.append(DLE);
    }
    dataToSend.append(crc);
    emit transmitData(dataToSend);
    statistic.txBytes += dataToSend.size();
    statistic.txPackets++;
    stateTimer->start(1000);
    //emit sendCommStatistic(statistic);
}

ProtocolManager::ReceiverState ProtocolManager::checkData(quint8 byte)
{
    ReceiverState state = ReceiverState::DATA_FLOW;
    currRxPack.data.append(byte);
    if(currTxPack.cmd == WRITE_PARAM) {
        currRxPack.cmd = WRITE_PARAM;
        currRxPack.addr = currTxPack.addr;
        state = ReceiverState::WAIT_CRC;
    }
    else if(currTxPack.cmd == READ_PARAM) {
        if(currRxPack.data.size() == (currTxPack.num + 1)) {
            currRxPack.cmd = READ_PARAM;
            currRxPack.addr = currTxPack.addr;
            state = ReceiverState::WAIT_CRC;
        }
    }
    return state;
}

ProtocolManager::ReceiverState ProtocolManager::checkCrc(quint8 byte)
{
    ReceiverState res = ReceiverState::FRAME_ERROR;
    if(computeCRC(currRxPack) == byte) {
        currRxPack.timestamp = QDateTime::currentMSecsSinceEpoch();
        emit packRecieved(currRxPack);
        res = ReceiverState::FREE;
        onValidPackReceive();
    }
    return res;
}

/**
 * @brief ProtocolManager::tryToStuffByte
 *  Попытка выполнить байт-стаффинг. Если БС не нужен, то
 * возвращается тот же самый байт, иначе его замена (без DLE)
 * @param byte байт данных
 * @return байт данных с учетом байт-стаффинга
 */
quint8 ProtocolManager::tryToStuffByte(quint8 byte)
{
    if((byte == STX) || (byte == DLE)) {
        return (byte + 0x20);
    }
    else {
        return byte;
    }
}

quint8 ProtocolManager::computeCRC(const SerialPacket &pack)
{
    quint8 crc = 0;
    QByteArray crcData;
    if(pack.type == SerialPacket::HOST) {
        crcData.append(pack.cmd);
        crcData.append(pack.addr & 0xFF);
        crcData.append((pack.addr >> 8) & 0xFF);
        crcData.append((pack.addr >> 16) & 0xFF);
        crcData.append((pack.addr >> 24) & 0xFF);
        crcData.append(pack.num);
    }
    for(int i = 0; i < pack.data.size(); i++) {
        crcData.append(pack.data[i]);
    }
    //crc = crc8_xxx(reinterpret_cast<quint8*>(crcData.data()), crcData.size());
    crc = crc8_sum(reinterpret_cast<quint8*>(crcData.data()), crcData.size());
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

quint8 ProtocolManager::crc8_sum(quint8 *pcBlock, quint8 len)
{
    quint8 crc = 0;
    for(quint8 i = 0; i < len; i++) {
        crc += pcBlock[i];
    }
    return (~crc);
}
