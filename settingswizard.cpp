#include "settingswizard.h"

#include <QTimer>
#include <QFile>

SettingsWizard::SettingsWizard(QObject *parent) : QObject(parent)
{
    mainSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "VSU", "display", this);
    backupSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "VSU", "display_backup", this);
    fBackupNeedUpdate = false;
    initErrorString = "";
    initSettings();
}

/**
 * @brief SettingsWizard::readValue
 *        Чтение параметра key из файла настроек
 * @param key
 * @param defaultValue значение по умолчанию
 * @return
 */
QVariant SettingsWizard::readValue(const QString &key, const QVariant &defaultValue)
{
    QVariant value = mainSettings->value(key, defaultValue);
    QSettings::Status status = mainSettings->status();
    //при чтении произошла какая-то ошибка
    if(status != QSettings::NoError) {
        emit sendError(convertErrorCodeToString(status));
    }
    return value;
}

/**
 * @brief SettingsWizard::readValue
 *        Чтение параметра key из массива данных файла настроек
 * @param arrayPrefix имя массива данных
 * @param index индекс в массиве данных
 * @param key
 * @param defaultValue значение по умолчанию
 * @return
 */
QVariant SettingsWizard::readValue(const QString &arrayPrefix, int index, const QString &key, const QVariant &defaultValue)
{
    QVariant value = readValueFromArray(mainSettings, arrayPrefix, index, key, defaultValue);
    QSettings::Status status = mainSettings->status();
    if(status != QSettings::NoError) {
        emit sendError(convertErrorCodeToString(status));
    }
    return value;
}

/**
 * @brief SettingsWizard::readValue
 *        Чтение параметра key из вложенного массива данных
 * @param arrayPrefix
 * @param index
 * @param subArrayPrefix
 * @param subIndex
 * @param key
 * @param defaultValue
 * @return
 */
QVariant SettingsWizard::readValue(const QString &arrayPrefix, int index, const QString &subArrayPrefix, int subIndex,
                                   const QString &key, const QVariant &defaultValue)
{
    QVariant value;
    int size = mainSettings->beginReadArray(arrayPrefix);
    if(index < size) {
        mainSettings->setArrayIndex(index);
        value = readValueFromArray(mainSettings, subArrayPrefix, subIndex, key, defaultValue);
    }
    else {
        value = defaultValue;
    }
    mainSettings->endArray();
    QSettings::Status status = mainSettings->status();
    if(status != QSettings::NoError) {
        emit sendError(convertErrorCodeToString(status));
    }
    return value;
}

/**
 * @brief SettingsWizard::delayedBackupUpdate
 *        Отложенное обновление резервного файла настроек приложения
 * @param delayMs
 */
void SettingsWizard::delayedBackupUpdate(int delayMs) {
    if((delayMs != 0) && (!fBackupNeedUpdate)) {
        fBackupNeedUpdate = true;
        QTimer::singleShot(delayMs, this, SLOT(updateBackupFile()));
    }
}

int SettingsWizard::getArraySize(const QString &arrayPrefix) {
    int size = mainSettings->beginReadArray(arrayPrefix);
    mainSettings->endArray();
    return size;
}

int SettingsWizard::getArraySize(const QString &arrayPrefix, int index, const QString &subArrayPrefix) {
    int size = mainSettings->beginReadArray(arrayPrefix);
    if(size) {
        mainSettings->setArrayIndex(index);
        size = mainSettings->beginReadArray(subArrayPrefix);
        mainSettings->endArray();
    }
    mainSettings->endArray();
    return size;
}

bool SettingsWizard::checkValueExist(const QString &key)
{
    return (mainSettings->contains(key));
}

/**
 * @brief SettingsWizard::saveValue
 * @param key
 * @param value
 */
void SettingsWizard::saveValue(const QString &key, const QVariant &value)
{
    mainSettings->setValue(key, value);
    mainSettings->sync();
    QSettings::Status status = mainSettings->status();
    if(status != QSettings::NoError) {
        emit sendError(convertErrorCodeToString(status));
    }
}

/**
 * @brief saveValue
 * @param arrayPrefix
 * @param index
 * @param key
 * @param defaultValue
 */
void SettingsWizard::saveValue(const QString &arrayPrefix, int index, const QString &key, const QVariant &value)
{
    mainSettings->beginWriteArray(arrayPrefix);
    mainSettings->setArrayIndex(index);
    mainSettings->setValue(key, value);
    mainSettings->sync();
    mainSettings->endArray();
    QSettings::Status status = mainSettings->status();
    if(status != QSettings::NoError) {
        emit sendError(convertErrorCodeToString(status));
    }
}

void SettingsWizard::saveValue(const QString &arrayPrefix, int index, const QString &subArrayPrefix, int subIndex,
                               const QString &key, const QVariant &value)
{
    mainSettings->beginWriteArray(arrayPrefix);
    mainSettings->setArrayIndex(index);
    mainSettings->beginWriteArray(subArrayPrefix);
    mainSettings->setArrayIndex(subIndex);
    mainSettings->setValue(key, value);
    mainSettings->sync();
    mainSettings->endArray();
    mainSettings->endArray();
    QSettings::Status status = mainSettings->status();
    if(status != QSettings::NoError) {
        emit sendError(convertErrorCodeToString(status));
    }
}

void SettingsWizard::removeValue(const QString &key)
{
    mainSettings->remove(key);
}

void SettingsWizard::removeValue(const QString &arrayPrefix, int index, const QString &key)
{
    int size = mainSettings->beginReadArray(arrayPrefix);
    if(index < size) {
        mainSettings->setValue("size", --size);
        mainSettings->setArrayIndex(index);
        mainSettings->remove(key);
    }
    mainSettings->endArray();
}

void SettingsWizard::removeValue(const QString &arrayPrefix, int index, const QString &subArrayPrefix, int subIndex, const QString &key)
{
    int size = mainSettings->beginReadArray(arrayPrefix);
    if(index < size) {
        mainSettings->setArrayIndex(index);
        removeValue(subArrayPrefix, subIndex, key);
    }
    mainSettings->endArray();
}

/**
 * @brief SettingsWizard::updateBackupFile
 * Сохраняем резервный файл настроек
 */
void SettingsWizard::updateBackupFile() {
    QSettings::Status status = mainSettings->status();
    //в случае возникновения ошибки не будем трогать backup
    if(status != QSettings::NoError) {
        fBackupNeedUpdate = false;
        return;
    }
    QFile backupTempFile("display_temp.ini");
    if(backupTempFile.open(QIODevice::ReadOnly)) {
        backupTempFile.remove();
    }
    QString nameStr = mainSettings->fileName();
    QFile file(nameStr);
    if(file.open(QIODevice::ReadOnly)) {
        if(file.copy(backupTempFile.fileName())) {
            file.close();
            nameStr = backupSettings->fileName();
            file.setFileName(backupSettings->fileName());
            if(file.open(QIODevice::ReadOnly)) {
                file.remove();
            }
            file.copy(backupTempFile.fileName(), backupSettings->fileName());
        }
        else {
            file.close();
        }
    }
    fBackupNeedUpdate = false;
}

/**
 * Private methods
 */

/**
 * @brief SettingsWizard::initSettings
 *        Инициализация настроек
 *       (включает в себя проверку целостности файла настроек)
 */
void SettingsWizard::initSettings()
{
    QTimer::singleShot(200, this, SLOT(checkInitError()));
    QStringList keysList = mainSettings->allKeys();
    //основной файл с настройками пуст
    if(keysList.isEmpty()) {
        keysList = backupSettings->allKeys();
        //резервный файл не пустой - копирование резервных настроек в основной файл
        if(!keysList.isEmpty()) {
            QFile file(mainSettings->fileName());
            if(file.open(QIODevice::ReadOnly)) {
                file.remove();
            }
            if(!file.copy(backupSettings->fileName(), mainSettings->fileName())) {
                initErrorString = "Fault to copy backup settings file!";
                return;
            }
            //после копирования производится повторное чтение файла настроек
            else {
                delete mainSettings;
                mainSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "VSU", "display", this);
                keysList = mainSettings->allKeys();
            }
        }
        else {
            //вполне реальная ситуация, совсем не обязательно, что это ошибка
            //по идее возможно только при первом запуске программы
            //!для отладки
            initErrorString = "Both settings file are empty!";
            return;
        }
    }
    //чтение всех параметров из основного файла настроек
    QStringList::const_iterator constIterator;
    for(constIterator = keysList.constBegin(); constIterator != keysList.constEnd(); ++constIterator) {
        volatile QVariant value = mainSettings->value(*constIterator);
        QSettings::Status status = mainSettings->status();
        //ошибка чтения - прекращение процедуры проверки
        if(status != QSettings::NoError) {
            initErrorString = convertErrorCodeToString(status);
            break;
        }
    }
}

/**
 * @brief SettingsWizard::checkInitError
 *        Отложенная проверка ошибки инициализации настроек
 */
void SettingsWizard::checkInitError() {
    if(!initErrorString.isEmpty()) {
        emit sendError(initErrorString);
    }
}

/**
 * @brief SettingsWizard::convertErrorCodeToString
 * @param code код статуса QSettings
 * @return errorString
 */
QString SettingsWizard::convertErrorCodeToString(QSettings::Status code)
{
    QString errorString = "";
    switch (code) {
    case QSettings::NoError:
        errorString = "NoError";
        break;
    case QSettings::AccessError:
        errorString = "AccessError";
        break;
    case QSettings::FormatError:
        errorString = "FormatError";
        break;
    default:
        errorString = "InvalidCodeError";
        break;
    }
    return errorString;
}

/**
 * @brief readValueFromArray
 *        Чтение параметра из массива с префиксом arrayPrefix
 * @param settings объект, осуществляющий работу с конфигурационным файлом
 * @param arrayPrefix имя массива
 * @param index индекс элемента в массиве
 * @param key имя параметра
 * @param defaultValue значение параметра по умолчанию
 * @return
 */
QVariant SettingsWizard::readValueFromArray(QSettings *settings, const QString &arrayPrefix, int index,
                            const QString &key, const QVariant &defaultValue)
{
    QVariant value;
    int size = settings->beginReadArray(arrayPrefix);
    if(index < size) {
        settings->setArrayIndex(index);
        value = settings->value(key, defaultValue);
    }
    else {
        value = defaultValue;
    }
    settings->endArray();
    return value;
}
