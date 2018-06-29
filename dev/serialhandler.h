#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

/**
 * @brief The SerialHandler class
 * QSerialPort with some extra signals and slots
 */

class SerialHandler : public QSerialPort
{
    Q_OBJECT
public:
    explicit SerialHandler(const QString &name = "");

    struct SerialSettings {
        QString name;
        qint32 baudRate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
    };

    void setSettings(const SerialSettings &settings);

public slots:
    void start();
    void stop();
    void putData(const QByteArray &data);

signals:
    void opened();
    void finished();
    void dataArrived(const QByteArray &data);
    void writingFailed();

private slots:
    void onReadyRead();
    void errorHandler(QSerialPort::SerialPortError error);

private:
    quint32 bRate;

};

#endif // SERIALHANDLER_H
