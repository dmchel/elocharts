#include "logger.h"

#include <QFile>

Logger::Logger(QObject *parent) : QObject(parent)
{

}

void Logger::createLogFile(const QString &name)
{
    if(name.isEmpty()) {
        return;
    }
    if(currentFile != Q_NULLPTR) {
        if(currentFile->isOpen()) {
            currentFile->close();
        }
        delete currentFile;
    }

    currentFile = new QFile(name, this);
    currentFile->open(QFile::ReadWrite);
}

void Logger::closeLogFile()
{

}

bool Logger::openLogFile(const QString &name)
{

}

void Logger::putLogData(const RawData &data)
{

}
