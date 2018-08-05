#include "customrecorddelegate.h"

#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
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
    //id
    if(index.column() == 0) {
        QSpinBox *editor = new QSpinBox(parent);
        editor->setFrame(false);
        editor->setRange(1, 255);
        editor->setSingleStep(1);
        return editor;
    }
    //period
    else if(index.column() == 2) {
        QSpinBox *editor = new QSpinBox(parent);
        editor->setFrame(false);
        editor->setRange(0, 0xFFFF);
        editor->setSingleStep(10);
        editor->setSuffix(" ms");
        return editor;
    }
    //factor and shift
    else if((index.column() == 3) || (index.column() == 4)) {
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setFrame(false);
        editor->setRange(-10e12, 10e12);
        editor->setDecimals(3);
        editor->setSingleStep(0.001);
        return editor;
    }
    //flags
    else if(index.column() == 7 || index.column() == 8) {
        QCheckBox *editor = new QCheckBox(parent);
        editor->setCheckable(true);
        return editor;
    }
    //graph color
    else if(index.column() == 9) {
        QComboBox *editor = new QComboBox(parent);
        editor->addItem("red", QColor(255, 0, 0));
        editor->addItem("green", QColor(0, 255, 0));
        editor->addItem("blue", QColor(0, 0, 255));
        editor->addItem("cyan", QColor(0, 255, 255));
        editor->addItem("magenta", QColor(255, 0, 255));
        editor->addItem("yellow", QColor(255, 255, 0));
        editor->addItem("orange", QColor(255, 128, 0));
        editor->addItem("white", QColor(Qt::white));
        editor->addItem("darkRed", QColor(Qt::darkRed));
        editor->addItem("darkGreen", QColor(Qt::darkGreen));
        editor->addItem("darkBlue", QColor(Qt::darkBlue));
        editor->addItem("darkCyan", QColor(Qt::darkCyan));
        editor->addItem("darkMagenta", QColor(Qt::darkMagenta));
        editor->addItem("darkYellow", QColor(Qt::darkYellow));
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
    if(index.column() == 0) {
        QSpinBox *idEdit = static_cast<QSpinBox *>(editor);
        int id = index.data().toInt();
        idEdit->setValue(id);
    }
    else if(index.column() == 2) {
        QSpinBox *periodEdit = static_cast<QSpinBox *>(editor);
        int period = index.data().toInt();
        periodEdit->setValue(period);
    }
    else if((index.column() == 3) || (index.column() == 4)) {
        QDoubleSpinBox *valEdit = static_cast<QDoubleSpinBox *>(editor);
        valEdit->setValue(index.data().toDouble());
    }
    else if((index.column() == 7) || (index.column() == 8)) {
        QCheckBox *checkEditor = static_cast<QCheckBox *>(editor);
        checkEditor->setChecked(index.data().toBool());
        checkEditor->setText(checkEditor->isChecked() ? ("true") : ("false"));
    }
    else if(index.column() == 9) {
        QComboBox *colorBox = static_cast<QComboBox *>(editor);
        //QColor color = index.data().value<QColor>();
        int idx = colorBox->findData(index.data());
        if(idx != -1) {
            colorBox->setCurrentIndex(idx);
        }
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
    if(index.column() == 0) {
        QSpinBox *idEdit = static_cast<QSpinBox *>(editor);
        model->setData(index, idEdit->value(), Qt::EditRole);
    }
    else if(index.column() == 2) {
        QSpinBox *periodEdit = static_cast<QSpinBox *>(editor);
        model->setData(index, periodEdit->value(), Qt::EditRole);
    }
    else if((index.column() == 3) || (index.column() == 4)) {
        QDoubleSpinBox *valEdit = static_cast<QDoubleSpinBox *>(editor);
        model->setData(index, valEdit->value(), Qt::EditRole);
    }
    else if((index.column() == 7) || (index.column() == 8)) {
        QCheckBox *checkEditor = static_cast<QCheckBox *>(editor);
        model->setData(index, checkEditor->isChecked(), Qt::EditRole);
    }
    else if(index.column() == 9) {
        QComboBox *colorBox = static_cast<QComboBox *>(editor);
        model->setData(index, colorBox->currentData(), Qt::EditRole);
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
    if(!index.isValid()) {
        return;
    }
    if(index.column() == 9) {
        painter->fillRect(option.rect, index.data(Qt::DisplayRole).value<QColor>());
    }
    QStyledItemDelegate::paint(painter, option, index);
}
