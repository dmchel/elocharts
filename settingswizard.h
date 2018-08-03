#ifndef SETTINGSWIZARD_H
#define SETTINGSWIZARD_H

#include <QObject>
#include <QSettings>

class SettingsWizard : public QObject
{
    Q_OBJECT
public:
    explicit SettingsWizard(QObject *parent = 0);

    void setApplicationName(const QString &name);
    void setOrganizationName(const QString &name);

    QVariant readValue(const QString &key, const QVariant &defaultValue);
    QVariant readValue(const QString &arrayPrefix, int index, const QString &key, const QVariant &defaultValue);
    QVariant readValue(const QString &arrayPrefix, int index, const QString &subArrayPrefix, int subIndex,
                       const QString &key, const QVariant &defaultValue);

    int getArraySize(const QString &arrayPrefix);
    int getArraySize(const QString &arrayPrefix, int index, const QString &subArrayPrefix);

    bool checkValueExist(const QString &key);

    QStringList readAllKeys();
    QStringList readAllGroups();

    void delayedBackupUpdate(int delayMs);

signals:
    void sendError(const QString &errStr);

public slots:
    void saveValue(const QString &key, const QVariant &value);
    void saveValue(const QString &arrayPrefix, int index, const QString &key, const QVariant &value);
    void saveValue(const QString &arrayPrefix, int index, const QString &subArrayPrefix, int subIndex,
                   const QString &key, const QVariant &value);

    void removeValue(const QString &key);
    void removeValue(const QString &arrayPrefix, int index, const QString &key);
    void removeValue(const QString &arrayPrefix, int index, const QString &subArrayPrefix, int subIndex, const QString &key);

    void updateBackupFile();

private:
    void initSettings();
    QVariant readValueFromArray(QSettings *settings, const QString &arrayPrefix, int index,
                                const QString &key, const QVariant &defaultValue);
    QString convertErrorCodeToString(QSettings::Status code);

private slots:
    void checkInitError();

private:
    QSettings *mainSettings;
    QSettings *backupSettings;
    QString initErrorString;
    bool fBackupNeedUpdate;

    QString orgName = "";
    QString appName = "";
};

#endif // SETTINGSWIZARD_H
