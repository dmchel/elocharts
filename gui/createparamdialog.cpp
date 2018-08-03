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

QJsonObject CreateParamDialog::jsonData() const
{
    QJsonObject jsonResult;
    jsonResult["id"] = ui->idSpinBox->value();
    jsonResult["name"] = ui->nameEdit->text();
    jsonResult["factor"] = ui->factorSpinBox->value();
    jsonResult["shift"] = ui->shiftSpinBox->value();
    jsonResult["period"] = ui->periodSpinBox->value();
    return jsonResult;
}
