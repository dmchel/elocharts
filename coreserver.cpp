#include "coreserver.h"

#include "dev/serialhandler.h"
#include "recordModel/chartrecordmodel.h"
#include "settingswizard.h"

#include <QThread>
#include <QTimer>
#include <QDebug>

CoreServer::CoreServer(QObject *parent) : QObject(parent)
{
    dataVault = new ProtocolData(this);

    connect(dataVault, &ProtocolData::dataArrived, this, &CoreServer::onNewChartData);

    chartModel = new ChartRecordModel(this);
    chartDelegate = new CustomRecordDelegate(this);
    settings = new SettingsWizard(this);

    checkConnectionTimer = new QTimer(this);
    connect(checkConnectionTimer, &QTimer::timeout, this, &CoreServer::checkConnection);
    checkConnectionTimer->start(100);

    ParamDataItem param_1(1, "PARAM_TEST_1", 100);
    ParamDataItem param_2(2, "PARAM_TEST_2", 100);
    ParamDataItem param_3(3, "PARAM_TEST_3", 100);
    chartModel->addRecord(param_1);
    chartModel->addRecord(param_2);
    chartModel->addRecord(param_3);
}

CoreServer::~CoreServer()
{

}

ChartRecordModel *CoreServer::dataModel() const
{
    return chartModel;
}

CustomRecordDelegate *CoreServer::dataDelegate() const
{
    return chartDelegate;
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
    settings.parity = QSerialPort::NoParity;
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

void CoreServer::readSettings()
{

}

void CoreServer::writeParamToSettings(int id)
{

}

void CoreServer::onNewChartData(RawData value)
{
    chartModel->updateRecord(value.id, value.dot.y());
    emit chartData(value.id, value.dot.x(), value.dot.y());
}

void CoreServer::checkConnection()
{
    if(protocol != Q_NULLPTR) {
        ProtocolManager::CommunicationStatistic statistic = protocol->commStatus();
        emit sendConnectionStatus(statistic.fConnected);
        QString infoStr = "Rx: " + QString::number(statistic.rxBytes) + "Tx: " + QString::number(statistic.txBytes) + " bytes";
        emit connectionInfoChanged(infoStr);
    }
}

