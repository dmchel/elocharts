#include "ledrecordmodel.h"

#include <QColor>

LedRecordItem::LedRecordItem() : num(0), timestamp(0), duration(0),
                                 ledColor("red"), ledData("0000")
{

}

LedRecordItem::LedRecordItem(int num_, int timestamp_, int duration_,
                             QString color, QString data)
{
    num = num_;
    timestamp = timestamp_;
    duration = duration_;
    ledColor = color;
    ledData = data;
}

LedRecordModel::LedRecordModel(QObject *parent) : QAbstractTableModel(parent)
{
    records.clear();
}

void LedRecordModel::addRecord(const LedRecordItem &record) {
    beginResetModel();
    records.append(record);
    endResetModel();
}

void LedRecordModel::removeRecord(const LedRecordItem &record) {
    beginResetModel();
    int index = records.indexOf(record, 0);
    if(index >= 0) {
        records.removeAt(index);
    }
    endResetModel();
}

void LedRecordModel::removeRecord(int index)
{
    beginResetModel();
    if((index >= 0) && (index < records.size())) {
        records.removeAt(index);
    }
    endResetModel();
}

void LedRecordModel::rewriteRecord(int index, const LedRecordItem &rwRecord)
{
    beginResetModel();
    if((index >= 0) && (index < records.size())) {
        records[index] = rwRecord;
    }
    endResetModel();
}

QList<LedRecordItem> LedRecordModel::readAllData()
{
    return records;
}

void LedRecordModel::removeAll() {
    beginResetModel();
    records.clear();
    endResetModel();
}

Qt::ItemFlags LedRecordModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if(index.isValid()) {
        theFlags |= Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    return theFlags;
}

int LedRecordModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : records.count();
}

int LedRecordModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : (MAX_COLUMN_NUM_LOG + 1);
}

QVariant LedRecordModel::data(const QModelIndex &index, int role) const {
    if((!index.isValid()) || (index.column() < 0) || (index.row() < 0) || (index.row() > records.count())
            || (index.column() > MAX_COLUMN_NUM_LOG)) {
        return QVariant();
    }
    if(records.isEmpty()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        const LedRecordItem &record = records.at(index.row());
        switch (index.column()) {
        case 0:
            return record.num;
        case 1:
            return record.timestamp;
        case 2:
            return record.duration;
        case 3:
            return record.ledColor;
        case 4:
            return record.ledData;
        }
    }
    /*else if(role == Qt::DecorationRole) {

    }*/
    return QVariant();
}

QVariant LedRecordModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole) {
        return QVariant();
    }
    if(orientation == Qt::Horizontal) {
        switch(section) {
        case 0:
            return tr("Number");
        case 1:
            return tr("Timestamp");
        case 2:
            return tr("Duration");
        case 3:
            return tr("LedColor");
        case 4:
            return tr("LedData");
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool LedRecordModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole) {
        return false;
    }
    if(index.isValid() && value.isValid()) {
        auto row = index.row();
        if(row < records.size()) {
            switch (index.column()) {
            case 0:
                records[row].num = value.toInt();
                return true;
            case 1:
                records[row].timestamp = value.toInt();
                return true;
            case 2:
                records[row].duration = value.toInt();
                return true;
            case 3:
                records[row].ledColor = value.toString();
                return true;
            case 4:
                records[row].ledData = value.toString();
                return true;
            }
        }
    }
    return false;
}

/**
  Private methods
 */

