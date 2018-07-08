#include "datetimedelegate.h"

#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QPainter>

CustomRecordDelegate::CustomRecordDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

QWidget *CustomRecordDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    if(!index.isValid()) {
        return Q_NULLPTR;
    }
    //color column
    if(index.column() == 1) {
        QSpinBox *editor = new QSpinBox(parent);
        editor->setFrame(false);
        editor->setRange(0, 1600000);
        editor->setSingleStep(100);
        editor->setSuffix(" ms");
        return editor;
    }
    else if(index.column() == 2) {
        QSpinBox *editor = new QSpinBox(parent);
        editor->setFrame(false);
        editor->setRange(0, 0xFFFF);
        editor->setSingleStep(10);
        editor->setSuffix(" ms");
        return editor;
    }
    else if(index.column() == 3) {
        QComboBox *editor = new QComboBox(parent);
        editor->setFrame(false);
        editor->addItem(tr("red"));
        editor->addItem(tr("yellow"));
        editor->addItem(tr("both"));
        return editor;
    }
    else {
        QLineEdit *editor = new QLineEdit(parent);
        return editor;
    }
}

void CustomRecordDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(!index.isValid()) {
        return;
    }
    //color column
    if(index.column() == 1) {
        QSpinBox *timestEdit = static_cast<QSpinBox *>(editor);
        int timestamp = index.data().toInt();
        timestEdit->setValue(timestamp);
    }
    else if(index.column() == 2) {
        QSpinBox *durationEdit = static_cast<QSpinBox *>(editor);
        int duration = index.data().toInt();
        durationEdit->setValue(duration);
    }
    else if(index.column() == 3) {
        QComboBox *colorEdit = static_cast<QComboBox *>(editor);
        QString colorString = index.data().toString();
        colorEdit->setCurrentText(colorString);
    }
    else {
        QLineEdit *textEdit = static_cast<QLineEdit *>(editor);
        textEdit->setText(index.data().toString());
    }
}

void CustomRecordDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(!index.isValid()) {
        return;
    }
    //color column
    if(index.column() == 1) {
        QSpinBox *timestEdit = static_cast<QSpinBox *>(editor);
        int timest = timestEdit->value();
        model->setData(index, timest, Qt::EditRole);
    }
    else if(index.column() == 2) {
        QSpinBox *durationEdit = static_cast<QSpinBox *>(editor);
        int duration = durationEdit->value();
        model->setData(index, duration, Qt::EditRole);
    }
    else if(index.column() == 3) {
         QComboBox *colorEdit = static_cast<QComboBox *>(editor);
         QString colorString = colorEdit->currentText();
         model->setData(index, colorString, Qt::EditRole);
    }
    else {
         QLineEdit *textEdit = static_cast<QLineEdit *>(editor);
         model->setData(index, textEdit->text(), Qt::EditRole);
    }
}

void CustomRecordDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

void CustomRecordDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    Q_UNUSED(option)
    if(!index.isValid()) {
        return;
    }
    if(index.column() == 3) {
        if(index.data(Qt::DisplayRole).toString() == "red") {
            painter->fillRect(option.rect, QColor(225, 15, 0, 200));
        }
        else if(index.data(Qt::DisplayRole).toString() == "yellow") {
            painter->fillRect(option.rect, QColor(225, 225, 0, 200));
        }
        else if(index.data(Qt::DisplayRole).toString() == "both") {
            painter->fillRect(option.rect, QColor(225, 100, 0, 200));
        }
    }
    QStyledItemDelegate::paint(painter, option, index);
}
