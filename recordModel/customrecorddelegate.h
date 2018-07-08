#ifndef DATETIMEDELEGATE_H
#define DATETIMEDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>

class CustomRecordDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    CustomRecordDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const;


private:

};

#endif // DATETIMEDELEGATE_H
