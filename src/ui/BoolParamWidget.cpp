#include "BoolParamWidget.h"
#include "ui_BoolParamWidget.h"

BoolParamWidget::BoolParamWidget(QWidget *parent, BoolParamE type)
    : QWidget(parent)
    , ui(new Ui::BoolParamWidget)
{
    ui->setupUi(this);

    this->type = type;

    switch (type) {
    case USE_FLASH_ATTENTION:
        init("Use Flash Attention", true); // TODO: use Qt's localization
        break;
    default:
        init("Unknown Parameter", false);
    }
}

BoolParamWidget::~BoolParamWidget()
{
    delete ui;
}

BoolParamWidget::BoolParamE BoolParamWidget::getType()
{
    return this->type;
}

bool BoolParamWidget::getValue()
{
    return ui->checkBox->isChecked();
}

void BoolParamWidget::init(const QString &label, bool defaultValue)
{
    ui->label->setText(label);
    ui->checkBox->setChecked(defaultValue);
}
