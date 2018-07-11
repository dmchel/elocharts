#include "coreserver.h"

#include "dev/serialhandler.h"
//#include "protocolmanager.h"
#include "recordModel/chartrecordmodel.h"

#include <QThread>
#include <QDebug>

CoreServer::CoreServer(QObject *parent) : QObject(parent)
{
    dataVault = new ProtocolData(this);

    connect(dataVault, &ProtocolData::dataArrived, this, &CoreServer::onNewChartData);

    chartModel = new ChartRecordModel(this);
}

CoreServer::~CoreServer()
{

}

ChartRecordModel *CoreServer::dataModel() const
{
    return chartModel;
}

/**
 * @brief CoreServer::openSerialPort
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
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    if(ports.isEmpty()) {
        qDebug() << "Serial ports not found!";
        return;
    }
    SerialHandler::SerialSettings settings;
    settings.baudRate = QSerialPort::Baud115200;
    settings.dataBits = QSerialPort::Data8;
    settings.flowControl = QSerialPort::NoFlowControl;
    settings.name = name;
    //settings.name = ports.at(0).portName();
    settings.parity = QSerialPort::EvenParity;
    settings.stopBits = QSerialPort::OneStop;
    serialDevice->setSettings(settings);

    QThread *serialThread = new QThread();
    serialDevice->moveToThread(serialThread);
    protocol->moveToThread(serialThread);
    //thread control
    connect(serialThread, &QThread::started, serialDevice, &SerialHandler::start);
    connect(serialThread, &QThread::finished, serialThread, &QThread::deleteLater);
    connect(serialDevice, &SerialHandler::opened, this, &CoreServer::onOpenSerialPort);
    connect(serialDevice, &SerialHandler::finished, serialThread, &QThread::quit);
    connect(serialDevice, &SerialHandler::finished, protocol, &ProtocolManager::stop);
    connect(serialDevice, &SerialHandler::finished, this, &CoreServer::onCloseSerialPort);
    connect(serialDevice, &SerialHandler::finished, serialDevice, &SerialHandler::deleteLater);
    connect(this, &CoreServer::stopSerial, serialDevice, &SerialHandler::stop);
    connect(protocol, &ProtocolManager::finished, serialThread, &QThread::quit);
    connect(protocol, &ProtocolManager::finished, protocol, &ProtocolManager::deleteLater);
    //data exchange
    connect(serialDevice, &SerialHandler::dataArrived, protocol, &ProtocolManager::receiveData);
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

void CoreServer::onNewChartData(RawData value)
{
    //chartModel->updateRecord();
    emit chartData(value.dot.x(), value.dot.y());
}

