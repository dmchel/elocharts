#include "coreserver.h"

#include "dev/serialhandler.h"
#include "recordModel/chartrecordmodel.h"
#include "settingswizard.h"

#include <QThread>
#include <QTimer>
#include <QJsonObject>
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

    readSettings();

    //ParamDataItem param_1(1, "PARAM_TEST_1", 100);
    //ParamDataItem param_2(2, "PARAM_TEST_2", 100);
    //ParamDataItem param_3(3, "PARAM_TEST_3", 100);
    //chartModel->addRecord(param_1);
    //chartModel->addRecord(param_2);
    //chartModel->addRecord(param_3);

    //writeParamToSettings(param_1);
    //writeParamToSettings(param_2);
    //writeParamToSettings(param_3);
}

CoreServer::~CoreServer()
{

}

void CoreServer::setSoftVersion(const QString &str)
{
    progVersion = str;
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
    connect(this, &CoreServer::sendUartTxData, serialDevice, &SerialHandler::putData);
    //connect(protocol, &ProtocolManager::sendCommStatistic, this, &CoreServer::updateConnectionInfo);
    connect(dataVault, &ProtocolData::generatePacket, protocol, &ProtocolManager::sendPacket);
    connect(protocol, &ProtocolManager::packRecieved, dataVault, &ProtocolData::packetHandler);

    serialThread->start(QThread::TimeCriticalPriority);
}

void CoreServer::closeSerialPort()
{
    emit stopSerial();
}

void CoreServer::shellListenUart()
{
    if(serialDevice != Q_NULLPTR) {
        if(fListenUart) {
            disconnect(serialDevice, &SerialHandler::dataArrived, this, &CoreServer::sendUartRxData);
        }
        else {
            connect(serialDevice, &SerialHandler::dataArrived, this, &CoreServer::sendUartRxData);
        }
        fListenUart = !fListenUart;
    }
    else {
        fListenUart = false;
    }
}

void CoreServer::shellUartStatus()
{
    if(protocol != Q_NULLPTR) {
        ProtocolManager::CommunicationStatistic stat = protocol->commStatus();
        QStringList statList;
        statList.append(tr("\r\nCommunication statistic: \r\n"));
        if(stat.fConnected) {
            statList.append(tr("SUZ online\r\n"));
        }
        else {
            statList.append(tr("SUZ offline\r\n"));
        }
        statList.append(tr("tx packs: ") + QString().setNum(stat.txPackets) + "\r\n");
        statList.append(tr("tx bytes: ") + QString().setNum(stat.txBytes) + "\r\n");
        statList.append(tr("rx packs: ") + QString().setNum(stat.rxPackets) + "\r\n");
        statList.append(tr("rx bytes: ") + QString().setNum(stat.txBytes) + "\r\n");
        statList.append(tr("Errors total: ") + QString().setNum(stat.overallErrors) + "\r\n");
        statList.append(tr("Crc errors: ") + QString().setNum(stat.crcErrors) + "\r\n");
        statList.append(tr("Timeouts: ") + QString().setNum(stat.timeoutErrors) + "\r\n");
        statList.append(tr("Format errors: ") + QString().setNum(stat.formatErrors) + "\r\n");
        //statList.append(tr("Stuff errors: ") + QString().setNum(stat.stuffErrors) + "\r\n");
        statList.append("\r\n");
        emit sendConsoleText(statList.join(""));
    }
}

void CoreServer::shellSendUart(const QByteArray &data)
{
    if(serialDevice != Q_NULLPTR) {
        emit sendUartTxData(data);
    }
}

void CoreServer::shellPrintPorts()
{
    QStringList portList;
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for(auto port : ports) {
        portList.append(port.portName() + " " + (port.isBusy() ? "busy " : "free "));
    }
    portList.append("\r\n");
    emit sendConsoleText(portList.join("\r\n"));
}

void CoreServer::shellVersionRequest()
{
    emit sendConsoleText("ELOCharts " + progVersion + ".\n");
}

/**
 * Private methods
 */

void CoreServer::onOpenSerialPort()
{
    emit connected();
    emit sendConsoleText("Serial port opened " + serialDevice->portName() + ".\r\n");
    qDebug() << "Serial port opened " + serialDevice->portName() + ".\r\n";
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
    QStringList groups = settings->readAllGroups();

    if(!groups.isEmpty()) {
        for(auto group : groups) {
            if(group.startsWith("parameter")) {
                QJsonObject paramData;
                paramData["id"] = settings->readValue(group + "/id", -1).toInt();
                paramData["name"] = settings->readValue(group + "/name", "unknown").toString();
                paramData["factor"] = settings->readValue(group + "/factor", 1.0).toDouble();
                paramData["shift"] = settings->readValue(group + "/shift", 0.0).toDouble();
                paramData["period"] = settings->readValue(group + "/period", 500).toInt();
                paramData["fShowGraph"] = settings->readValue(group + "/fShowGraph", false).toBool();
                paramData["color"] = QJsonValue::fromVariant(settings->readValue(group + "/color", QColor("blue")));
                chartModel->addRecord(paramData);
            }
        }
    }
    else {
        groups.clear();
    }
}

void CoreServer::writeParamToSettings(const ParamDataItem &item)
{
    QJsonObject paramData;
    paramData["id"] = item.id;
    paramData["name"] = item.name;
    paramData["factor"] = item.factor;
    paramData["shift"] = item.shift;
    paramData["period"] = item.period;
    paramData["fShowGraph"] = item.fShowGraph;
    paramData["color"] = QJsonValue::fromVariant(QVariant(item.graphColor));
    QString prefix = "parameter" + QString::number(paramData["id"].toInt()) + "/";
    for(auto key : paramData.keys()) {
        settings->saveValue(prefix + key, paramData[key].toVariant());
    }
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

