#ifndef LEDRECORDMODEL_H
#define LEDRECORDMODEL_H

#include <QAbstractTableModel>
#include <QColor>
#include <QString>
#include <QList>

class ParamDataItem
{
public:
    ParamDataItem();
    ParamDataItem(int pId, const QString &pName, int pPeriod,
                  qreal pFactor = 1.0, qreal pShift = 0.0, qreal pValue = 0.0,
                  int pRawValue = 0, bool isActive = false, bool fGraph = false,
                  QColor color = QColor(Qt::blue));

    int id;             //идентификатор параметра
    QString name;       //имя параметра
    int period;         //период обновления значения
    qreal factor;       //коэффициент перевода из сырых данных в данные представления
    qreal shift;        //сдвиг сырых данных
    qreal value;        //финальное значение параметра
    int rawValue;       //"сырое" значение параметра
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
    explicit ChartRecordModel(QObject* parent = 0);
    //~ChartRecordModel();

    void addRecord(const ParamDataItem &record);
    void updateRecord(int id, int value);
    void removeRecord(const ParamDataItem &record);
    void removeRecord(int index);
    void rewriteRecord(int index, const ParamDataItem &rwRecord);

    ParamDataItem recordById(int id);

    QList<ParamDataItem> readAllData();
    void removeAll();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role);

private:
    bool findRecord(int id, ParamDataItem *dest);

private:
    QList<ParamDataItem> records;

    const int MAX_COLUMN_NUM = 9;

};

#endif // LEDRECORDMODEL_H
