#include "shell.h"

#include <QString>
#include <QStringList>

Shell::Shell(QObject *parent) : QObject(parent)
{

}

void Shell::hNewCommand(const QString &cmd)
{
    if(cmd.size()) {
        commandParser(cmd);
    }
}

/**
 * @brief Shell::receiveData
 *  Получены новые данные из последовательного порта
 * @param data
 */
void Shell::receiveData(const QByteArray &data)
{
    if(fEcho) {
        emit eSendUart(data);
    }
}

/**
 * Private methods
 */

/**
 * @brief Shell::commandParser
 *        Парсер команд
 * @param cmd команда(пробел)флаг1(пробел)(...)флаг10(пробел)параметр1(...)параметр10
 */
void Shell::commandParser(const QString &cmd)
{
    int index = cmd.indexOf(' ');
    //в команде есть флаги или параметры
    if(index != -1) {
        QStringList splitList = cmd.split(' ', QString::SkipEmptyParts);
        QString name = splitList.at(0);
        QByteArray data;
        for(int i = 1; i < splitList.size(); i++) {
            QString part = splitList.at(i);
            //этот параметр - строка
            if(part.at(0) == '\"') {
                //строка без кавычек
                data.append(part.remove('\"'));
            }
            //а это бинарные данные
            else if(part.at(0) != '-') {
                bool fOk = false;
                short num = 0;
                if(part.left(2) == "0x") {
                    num = part.toShort(&fOk, 16);
                }
                else {
                    num = part.toShort(&fOk, 10);
                }
                if(fOk) {
                    data.append(num);
                }
            }
        }
        if(shellCommands.contains(name)) {
            commandExecuter(shellCommands.value(name), data);
        }
        else {
            emit eUnknownCommand(cmd);
        }
    }
    //команда без дополнительных параметров
    else {
        if(shellCommands.contains(cmd)) {
            commandExecuter(shellCommands.value(cmd));
        }
        else {
            emit eUnknownCommand(cmd);
        }
    }
}

void Shell::commandExecuter(ShellCMD cmd)
{
    switch(cmd) {
    case ShellCMD::STOP:
        emit eStopAll();
        break;
    case ShellCMD::HELP:
        printHelp();
        break;
    case ShellCMD::CLEAR:
        emit eClear();
        break;
    case ShellCMD::LISTEN_UART:
        emit eListenUart();
        break;
    case ShellCMD::SEND_UART:
        emit ePrintString("Command execute error!\n");
        break;
    case ShellCMD::STATUS_UART:
        emit eStatusUart();
        break;
    case ShellCMD::LIST_OF_PORTS:
        emit ePortList();
        break;
    case ShellCMD::ECHO_MODE:
        fEcho = !fEcho;
        break;
    case ShellCMD::WHO:
        emit eWhoIs();
        break;
    case ShellCMD::VER:
        emit eVersionRequest();
        break;
    default:
        //такого быть не должно
        emit ePrintString("Unexpected command!\n");
        break;
    }
}

void Shell::commandExecuter(ShellCMD cmd, const QByteArray & data)
{
    if(cmd == ShellCMD::SEND_UART) {
        emit eSendUart(data);
    }
    else if(cmd == ShellCMD::SET_BOARD_TYPE) {
        emit eBoardType(data);
    }
    else {
        emit ePrintString("Command execute error!\n");
    }
}

void Shell::printHelp()
{
    QString helpString = "\nELOChart shell ver. 1.00\n\n";
    helpString += tr("clc - очистить консоль\n");
    helpString += tr("lsuart - включить/выключить прослушивание uart\n");
    helpString += tr("sendu [d0] [d1] .... [dn] - отправка данных в uart\n");
    helpString += tr("stuart - вывод состояния uart\n");
    helpString += tr("lsport - список доступных последовательных портов в системе\n");
    helpString += tr("echo - режим эхо (активируется только при включенном прослушивании uart)\n");
    helpString += tr("stop - остановка работы shell\n");
    helpString += tr("ver - запрос версии ПО\n\n");
    emit ePrintString(helpString);
}
