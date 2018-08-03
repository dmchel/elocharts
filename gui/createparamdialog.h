#ifndef CREATEPARAMDIALOG_H
#define CREATEPARAMDIALOG_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class CreateParamDialog;
}

class CreateParamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateParamDialog(QWidget *parent = 0);
    ~CreateParamDialog();

    QJsonObject jsonData() const;

private:
    Ui::CreateParamDialog *ui;
};

#endif // CREATEPARAMDIALOG_H
