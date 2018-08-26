#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

#include "protocoldata.h"

class QFile;

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr);

    void createLogFile(const QString &name);
    void closeLogFile();
    bool openLogFile(const QString &name);

signals:

public slots:
    void putLogData(const RawData &data);

private:
    QFile *currentFile = Q_NULLPTR;

};

#endif // LOGGER_H
