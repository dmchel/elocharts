#include "chartrecordmodel.h"

ParamDataItem::ParamDataItem()
{
    id = 0;
    name = "none";
    period = 0;
    factor = 0.0;
    shift = 0.0;
    value = 0.0;
    rawValue = 0;
    fActive = false;
    fShowGraph = false;
    graphColor = QColor(0, 0, 0);
}

ParamDataItem::ParamDataItem(int pId, const QString &pName, int pPeriod,
                             qreal pFactor, qreal pShift, qreal pValue,
                             int pRawValue, bool isActive, bool fGraph,
                             QColor color)
{
    id = pId;
    name = pName;
    period = pPeriod;
    factor = pFactor;
    shift = pShift;
    value = pValue;
    rawValue = pRawValue;
    fActive = isActive;
    fShowGraph = fGraph;
    graphColor = color;
}

ChartRecordModel::ChartRecordModel(QObject *parent) : QAbstractTableModel(parent)
{
    records.clear();
}

void ChartRecordModel::addRecord(const ParamDataItem &record) {
    beginResetModel();
    records.append(record);
    endResetModel();
}

/**
 * @brief ChartRecordModel::updateRecord
 *  Обновить значение записи если она существует (проверка по id),
 * либо добавить новую запись, если такой записи еще нет в списке
 * @param record
 */
void ChartRecordModel::updateRecord(const ParamDataItem &record)
{
    beginResetModel();
    bool fReplaced = false;
    for(auto &rec : records) {
        if(rec.id == record.id) {
            rec = record;
            fReplaced = true;
        }
    }
    if(!fReplaced) {
        records.append(record);
    }
    endResetModel();
}

void ChartRecordModel::removeRecord(const ParamDataItem &record) {
    beginResetModel();
    int index = records.indexOf(record, 0);
    if(index >= 0) {
        records.removeAt(index);
    }
    endResetModel();
}

void ChartRecordModel::removeRecord(int index)
{
    beginResetModel();
    if((index >= 0) && (index < records.size())) {
        records.removeAt(index);
    }
    endResetModel();
}

void ChartRecordModel::rewriteRecord(int index, const ParamDataItem &rwRecord)
{
    beginResetModel();
    if((index >= 0) && (index < records.size())) {
        records[index] = rwRecord;
    }
    endResetModel();
}

QList<ParamDataItem> ChartRecordModel::readAllData()
{
    return records;
}

void ChartRecordModel::removeAll() {
    beginResetModel();
    records.clear();
    endResetModel();
}

Qt::ItemFlags ChartRecordModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if(index.isValid()) {
        theFlags |= Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    return theFlags;
}

int ChartRecordModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : records.count();
}

int ChartRecordModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : (MAX_COLUMN_NUM + 1);
}

QVariant ChartRecordModel::data(const QModelIndex &index, int role) const {
    if((!index.isValid()) || (index.column() < 0) || (index.row() < 0) || (index.row() > records.count())
            || (index.column() > MAX_COLUMN_NUM)) {
        return QVariant();
    }
    if(records.isEmpty()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        const ParamDataItem &record = records.at(index.row());
        switch (index.column()) {
        case 0:
            return record.id;
        case 1:
            return record.name;
        case 2:
            return record.period;
        case 3:
            return record.factor;
        case 4:
            return record.shift;
        case 5:
            return record.value;
        case 6:
            return record.rawValue;
        case 7:
            return record.fActive;
        case 8:
            return record.fShowGraph;
        case 9:
            return record.graphColor;
        }
    }
    /*else if(role == Qt::DecorationRole) {

    }*/
    return QVariant();
}

QVariant ChartRecordModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole) {
        return QVariant();
    }
    if(orientation == Qt::Horizontal) {
        switch(section) {
        case 0:
            return tr("id");
        case 1:
            return tr("Имя");
        case 2:
            return tr("Период");
        case 3:
            return tr("Коэффициент");
        case 4:
            return tr("Сдвиг");
        case 5:
            return tr("Значение");
        case 6:
            return tr("Сырое значение");
        case 7:
            return tr("Активен");
        case 8:
            return tr("Показать график");
        case 9:
            return tr("Цвет");
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool ChartRecordModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole) {
        return false;
    }
    if(index.isValid() && value.isValid()) {
        auto row = index.row();
        if(row < records.size()) {
            switch (index.column()) {
            case 0:
                records[row].id = value.toInt();
                return true;
            case 1:
                records[row].name = value.toString();
                return true;
            case 2:
                records[row].period = value.toInt();
                return true;
            case 3:
                records[row].factor = value.toDouble();
                return true;
            case 4:
                records[row].shift = value.toDouble();
                return true;
            case 5:
                records[row].value = value.toDouble();
                return true;
            case 6:
                records[row].rawValue = value.toInt();
                return true;
            case 7:
                records[row].fActive = value.toBool();
                return true;
            case 8:
                records[row].fShowGraph = value.toBool();
                return true;
            case 9:
                records[row].graphColor = value.value<QColor>();
                return true;
            }
        }
    }
    return false;
}

/**
  Private methods
 */

