#include "serialhandler.h"

SerialHandler::SerialHandler(const QString &name)
{
    setPortName(name);
    bRate = QSerialPort::Baud115200;

    connect(this, &SerialHandler::errorOccurred, this, &SerialHandler::errorHandler);
    connect(this, &SerialHandler::readyRead, this, &SerialHandler::onReadyRead);
}

void SerialHandler::setSettings(const SerialSettings &settings)
{
    setPortName(settings.name);
    bRate = settings.baudRate;
    setDataBits(settings.dataBits);
    setParity(settings.parity);
    setFlowControl(settings.flowControl);
    setStopBits(settings.stopBits);
}

/**
 * @brief SerialHandler::start
 * Запуск работы обработчика и открытие порта
 * (в случае если порт еще не открыт)
 */
void SerialHandler::start()
{
    if(!this->isOpen()) {
        if(this->open(QIODevice::ReadWrite)) {
            if(!setBaudRate(bRate)) {
                this->close();
                emit finished();
            }
            //порт с заданными параметрами готов к работе
            else {
                emit opened();
            }
        }
        else {
            emit finished();
        }
    }
}

/**
 * @brief SerialHandler::stop
 * Завершение работы обработчика порта
 */
void SerialHandler::stop()
{
    if(this->isOpen()) {
        this->close();
    }
    emit finished();
}

/**
 * @brief SerialHandler::putData
 *  Отправить данные в последовательный порт
 * @param data данные для отправки
 */
void SerialHandler::putData(const QByteArray &data)
{
    auto count = write(data);
    if((count == -1) || (count < data.size())) {
        emit writingFailed();
    }
}


/**
 * Private methods
 */

void SerialHandler::onReadyRead()
{
    QByteArray bytes = readAll();
    if(!bytes.isEmpty()) {
        emit dataArrived(bytes);
    }
}

void SerialHandler::errorHandler(QSerialPort::SerialPortError error)
{
    //потеря доступа к ресурсу
    if(error == QSerialPort::ResourceError) {
        this->close();
        emit finished();
    }
}
