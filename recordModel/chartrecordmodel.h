#ifndef LEDRECORDMODEL_H
#define LEDRECORDMODEL_H

#include <QAbstractTableModel>
#include <QColor>
#include <QString>
#include <QList>
#include <QJsonObject>
#include <QSortFilterProxyModel>

class ParamDataItem
{
public:
    ParamDataItem();
    ParamDataItem(int pId, const QString &pName, int pPeriod,
                  qreal pFactor = 1.0, qreal pShift = 0.0, qreal pValue = 0.0,
                  quint32 pRawValue = 0, bool isActive = false, bool fGraph = false,
                  QColor color = QColor(Qt::blue));
    ParamDataItem(const QJsonObject &jsonData);

    int id;             //идентификатор параметра
    QString name;       //имя параметра
    int period;         //период обновления значения
    qreal factor;       //коэффициент перевода из сырых данных в данные представления
    qreal shift;        //сдвиг сырых данных
    qreal value;        //финальное значение параметра
    quint32 rawValue;   //"сырое" значение параметра
    bool fActive;       //параметр активен (производится запрос новых данных)
    bool fShowGraph;    //флаг графического представления параметра
    QColor graphColor;  //цвет графика

    inline bool operator ==(const ParamDataItem &other) {
        return ((id == other.id) && (name == other.name));
    }
};

/**
 * @brief The ChartRecordModel class
 *  Модель данных для табличного отображения парамеров
 */
class ChartRecordModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum ColumnNumbers {
        COL_ID = 0,
        COL_NAME = 1,
        COL_PERIOD = 2,
        COL_FACTOR = 3,
        COL_SHIFT = 4,
        COL_VALUE = 5,
        COL_RAW_VALUE = 6,
        COL_ACTIVE = 7,
        COL_SHOW_GRAPH = 8,
        COL_COLOR = 9
    };

    explicit ChartRecordModel(QObject* parent = 0);
    //~ChartRecordModel();

    void addRecord(const ParamDataItem &record);
    void removeRecord(const ParamDataItem &record);
    void removeRecord(int index);
    void rewriteRecord(int index, const ParamDataItem &rwRecord);

    void updateRecordValue(int id, quint32 value);
    void updateRecordColor(int id, const QColor &color);
    ParamDataItem recordById(int id);

    QList<ParamDataItem> readAllData();
    void removeAll();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role);

signals:
    void recordChanged(const QModelIndex &index);

private:
    ParamDataItem *getRecordRefById(int id);
    int getRecordRowById(int id);

private:
    QList<ParamDataItem> records;

    const int MAX_COLUMN_NUM = 9;

};

class ChartRecordProxyModel : public QSortFilterProxyModel
{
public:
    explicit ChartRecordProxyModel(QObject *parent = 0);

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

};

#endif // LEDRECORDMODEL_H
