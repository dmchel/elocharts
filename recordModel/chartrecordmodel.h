#ifndef LEDRECORDMODEL_H
#define LEDRECORDMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QList>

class LedRecordItem
{
public:
    LedRecordItem();
    LedRecordItem(int num_, int timestamp_, int duration_,
                  QString color, QString data);

    int num;            //номер записи
    int timestamp;      //отметка времени, мс
    int duration;       //длительность, мс
    QString ledColor;   //цвет светодиодов
    QString ledData;    //данные о расположении светодиодов в формате %fretNum0%fretNum1...%fretNumX
                        //(X - номер старшей струны)

    inline bool operator ==(const LedRecordItem &other) {
        return (num == other.num) && (timestamp == other.timestamp) && (duration == other.duration) &&
               (ledColor == other.ledColor) && (ledData == other.ledData);
    }
};

class LedRecordModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit LedRecordModel(QObject* parent = 0);
    //~LogRecordModel();

    void addRecord(const LedRecordItem &record);
    void removeRecord(const LedRecordItem &record);
    void removeRecord(int index);
    void rewriteRecord(int index, const LedRecordItem &rwRecord);

    QList<LedRecordItem> readAllData();
    void removeAll();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role);

private:
    QList<LedRecordItem> records;

private:
    const int MAX_COLUMN_NUM_LOG = 4;

};

#endif // LEDRECORDMODEL_H
