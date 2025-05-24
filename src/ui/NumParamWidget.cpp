#include "NumParamWidget.h"
#include "ui_NumParamWidget.h"

NumParamWidget::NumParamWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NumParamWidget)
{
    ui->setupUi(this);

}

NumParamWidget::~NumParamWidget()
{
    delete ui;
}

void NumParamWidget::on_lineEdit_textChanged(const QString &arg1)
{
    ui->horizontalSlider->setValue(arg1.toInt());
}


void NumParamWidget::on_horizontalSlider_valueChanged(int value)
{
    ui->lineEdit->setText(QString::number(value));
}
