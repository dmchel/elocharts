#ifndef SHELL_H
#define SHELL_H

#include <QObject>
#include <QMap>

enum class ShellCMD {
    HELP,               //справка по шеллу
    CLEAR,              //очистить вывод
    STOP,               //остановить шелл
    LISTEN_UART,        //прослушка uart'а
    SEND_UART,          //отправка данных в uart
    STATUS_UART,        //состояние uart
    LIST_OF_PORTS,      //список доступных поротов
    ECHO_MODE,          //режим эхо
    SET_BOARD_TYPE,     //установка типа устройства
    WHO,                //запрос типа устройства,
    VER                 //запрос версии ПО
    //TODO
};

class Shell : public QObject
{
    Q_OBJECT
public:
    explicit Shell(QObject *parent = nullptr);

signals:
    void eUnknownCommand(const QString &cmd);
    void ePrintString(const QString &str);

    //command signals
    void eStopAll();
    void eClear();
    void eListenUart();
    void eSendUart(const QByteArray &data);
    void eStatusUart();
    void ePortList();
    void eBoardType(const QString &str);
    void eWhoIs();
    void eVersionRequest();

public slots:
    void hNewCommand(const QString &cmd);
    void receiveData(const QByteArray &data);

private:
    void commandParser(const QString &cmd);
    void commandExecuter(ShellCMD cmd);
    void commandExecuter(ShellCMD cmd, const QByteArray & data);
    void printHelp();

private:
    const QMap<QString, ShellCMD> shellCommands = {
        {"help", ShellCMD::HELP},
        {"clc", ShellCMD::CLEAR},
        {"stop", ShellCMD::STOP},
        {"lsuart", ShellCMD::LISTEN_UART},
        {"sendu", ShellCMD::SEND_UART},
        {"stuart", ShellCMD::STATUS_UART},
        {"lsport", ShellCMD::LIST_OF_PORTS},
        {"echo", ShellCMD::ECHO_MODE},
        //{"setbrd", ShellCMD::SET_BOARD_TYPE},
        //{"who", ShellCMD::WHO},
        {"ver", ShellCMD::VER}
    };

    bool fEcho = false;
};

#endif // SHELL_H
