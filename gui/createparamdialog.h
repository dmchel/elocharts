#ifndef CREATEPARAMDIALOG_H
#define CREATEPARAMDIALOG_H

#include <QDialog>

namespace Ui {
class CreateParamDialog;
}

class CreateParamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateParamDialog(QWidget *parent = 0);
    ~CreateParamDialog();

private:
    Ui::CreateParamDialog *ui;
};

#endif // CREATEPARAMDIALOG_H
