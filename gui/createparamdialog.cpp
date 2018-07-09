#include "createparamdialog.h"
#include "ui_createparamdialog.h"

CreateParamDialog::CreateParamDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateParamDialog)
{
    ui->setupUi(this);
}

CreateParamDialog::~CreateParamDialog()
{
    delete ui;
}
