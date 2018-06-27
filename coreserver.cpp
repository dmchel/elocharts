#include "coreserver.h"

#include "dev/serialhandler.h"
//#include "protocolmanager.h"
#include "protocoldata.h"

#include <QDebug>

CoreServer::CoreServer(QObject *parent) : QObject(parent)
{
    dataVault = new ProtocolData(this);
}

CoreServer::~CoreServer()
{

}

/**
 * @brief CoreServer::openSerialPort
 *
 * @param name
 */
void CoreServer::openSerialPort(const QString &name)
{
    //порт уже открыт?
    if(protocol != Q_NULLPTR) {
        return;
    }
    protocol = new ProtocolManager();
    serialDevice = new SerialHandler(name);
    SerialHandler::SerialSettings settings;
    settings.baudRate = QSerialPort::Baud115200;
    settings.dataBits = QSerialPort::Data8;
    settings.flowControl = QSerialPort::NoFlowControl;
    settings.name = name;
    settings.parity = QSerialPort::EvenParity;
    settings.stopBits = QSerialPort::OneStop;
    serialDevice->setSettings(settings);

    QThread *serialThread = new QThread();
    serialDevice->moveToThread(serialThread);
    protocol->moveToThread(serialThread);

    connect(serialThread, &QThread::started, serialDevice, &SerialHandler::start);
    connect(serialDevice, &SerialHandler::finished, serialThread, &QThread::quit);
    connect(serialDevice, &SerialHandler::opened, this, &CoreServer::onOpenSerialPort);
    connect(serialDevice, &SerialHandler::finished, this, &CoreServer::onCloseSerialPort);
    connect(this, &CoreServer::stopSerial, serialDevice, &SerialHandler::stop);
    connect(serialDevice, &SerialHandler::finished, serialDevice, &SerialHandler::deleteLater);
    connect(serialThread, &QThread::finished, serialThread, &QThread::deleteLater);
    connect(serialDevice, &SerialHandler::dataArrived, protocol, &ProtocolManager::receiveData);
    connect(protocol, &ProtocolManager::finished, serialThread, &QThread::quit);
    connect(protocol, &ProtocolManager::finished, protocol, &ProtocolManager::deleteLater);
    connect(protocol, &ProtocolManager::transmitData, serialDevice, &SerialHandler::putData);
    //connect(protocol, &ProtocolManager::sendCommStatistic, this, &CoreServer::updateConnectionInfo);

    connect(dataVault, &ProtocolData::generatePacket, protocol, &ProtocolManager::sendPacket);
    connect(protocol, &ProtocolManager::packRecieved, dataVault, &ProtocolData::packetHandler);

    serialThread->start(QThread::TimeCriticalPriority);
}

void CoreServer::closeSerialPort()
{
    emit stopSerial();
}

/**
 * Private methods
 */

void CoreServer::onOpenSerialPort()
{
    emit connected();
    qDebug() << "Serial port opened.";
}

void CoreServer::onCloseSerialPort()
{
    emit disconnected();
    protocol = Q_NULLPTR;
    serialDevice = Q_NULLPTR;
    qDebug() << "Serial port closed.";
}
