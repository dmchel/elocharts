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
                             quint32 pRawValue, bool isActive, bool fGraph,
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

ParamDataItem::ParamDataItem(const QJsonObject &jsonData)
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
    if(!jsonData.isEmpty()) {
        id = jsonData["id"].toInt();
        name = jsonData["name"].toString();
        period = jsonData["period"].toInt();
        factor = jsonData["factor"].toDouble();
        shift = jsonData["shift"].toDouble();
        fShowGraph = jsonData["fShowGraph"].toBool();
        graphColor = jsonData["color"].toVariant().value<QColor>();
    }
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
 *  Обновить значение записи если она существует (проверка по id)
 * @param record
 */
void ChartRecordModel::updateRecordValue(int id, quint32 value)
{
    ParamDataItem *pItem = getRecordRefById(id);
    if(pItem != Q_NULLPTR) {
        pItem->rawValue = value;
        pItem->value = value * pItem->factor + pItem->shift;
        int row = getRecordRowById(id);
        emit recordChanged(this->index(row, 6));
        emit recordChanged(this->index(row, 5));
    }
}

void ChartRecordModel::updateRecordColor(int id, const QColor &color)
{
    ParamDataItem *pItem = getRecordRefById(id);
    if(pItem != Q_NULLPTR) {
        pItem->graphColor = color;
        int row = getRecordRowById(id);
        emit recordChanged(this->index(row, 9));
    }
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

ParamDataItem ChartRecordModel::recordById(int id)
{
    ParamDataItem item;
    ParamDataItem *pItem = getRecordRefById(id);
    if(pItem != Q_NULLPTR) {
        item = *pItem;
    }
    return item;
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
        case COL_ID:
            return record.id;
        case COL_NAME:
            return record.name;
        case COL_PERIOD:
            return record.period;
        case COL_FACTOR:
            return record.factor;
        case COL_SHIFT:
            return record.shift;
        case COL_VALUE:
            return record.value;
        case COL_RAW_VALUE:
            return record.rawValue;
        case COL_ACTIVE:
            return record.fActive;
        case COL_SHOW_GRAPH:
            return record.fShowGraph;
        case COL_COLOR:
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
        case COL_ID:
            return tr("id");
        case COL_NAME:
            return tr("Имя");
        case COL_PERIOD:
            return tr("Период");
        case COL_FACTOR:
            return tr("Коэффициент");
        case COL_SHIFT:
            return tr("Сдвиг");
        case COL_VALUE:
            return tr("Значение");
        case COL_RAW_VALUE:
            return tr("Сырое значение");
        case COL_ACTIVE:
            return tr("Активен");
        case COL_SHOW_GRAPH:
            return tr("Вывод на график");
        case COL_COLOR:
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
        bool fOk = false;
        auto row = index.row();
        if(row < records.size()) {
            switch (index.column()) {
            case COL_ID:
                records[row].id = value.toInt();
                fOk = true;
                break;
            case COL_NAME:
                records[row].name = value.toString();
                fOk = true;
                break;
            case COL_PERIOD:
                records[row].period = value.toInt();
                fOk = true;
                break;
            case COL_FACTOR:
                records[row].factor = value.toDouble();
                fOk = true;
                break;
            case COL_SHIFT:
                records[row].shift = value.toDouble();
                fOk = true;
                break;
            case COL_VALUE:
                records[row].value = value.toDouble();
                records[row].rawValue = (value.toDouble() / records[row].factor) - records[row].shift;
                fOk = true;
                break;
            case COL_RAW_VALUE:
                records[row].rawValue = value.toUInt();
                fOk = true;
                break;
            case COL_ACTIVE:
                records[row].fActive = value.toBool();
                fOk = true;
                break;
            case COL_SHOW_GRAPH:
                records[row].fShowGraph = value.toBool();
                fOk = true;
                break;
            case COL_COLOR:
                records[row].graphColor = value.value<QColor>();
                fOk = true;
                break;
            default:
                break;
            }
            emit dataChanged(index, index);
            return fOk;
        }
    }
    return false;
}

/**
  Private methods
 */

/**
 * @brief ChartRecordModel::getRecordRefById
 *  Получить указатель на запись с идентификатором id.
 * Если такой записи не существует возвращается Q_NULLPTR.
 * @param id
 * @return pItem указатель на запись
 */
ParamDataItem *ChartRecordModel::getRecordRefById(int id)
{
    ParamDataItem *pItem = Q_NULLPTR;
    int i = 0;
    for(auto &rec : records) {
        if(rec.id == id) {
            pItem = &records[i];
        }
        i++;
    }
    return pItem;
}

int ChartRecordModel::getRecordRowById(int id)
{
    int row = 0;
    for(auto &rec : records) {
        if(rec.id == id) {
            break;
        }
        row++;
    }
    return row;
}


ChartRecordProxyModel::ChartRecordProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}

bool ChartRecordProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QVariant leftData = sourceModel()->data(source_left);
    QVariant rightData = sourceModel()->data(source_right);
    if((source_left.column() == ChartRecordModel::COL_ID)) {
        auto left = leftData.toInt();
        auto right = rightData.toInt();
        return (left < right);
    }
    else {
        QString leftString = leftData.toString();
        QString rightString = rightData.toString();
        return (QString::compare(leftString, rightString, Qt::CaseInsensitive) < 0);
    }
}

