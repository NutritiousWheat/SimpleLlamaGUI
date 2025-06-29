#include "BoolParamWidget.h"
#include "ui_BoolParamWidget.h"

QMap<BoolParamWidget::BoolParamType, QString> BoolParamWidget::names // TODO: use Qt's localization
    = {{USE_FLASH_ATTENTION, "Use Flash Attention"},

    {BOOL_PARAM_COUNT, "Unknown bool param"}};


QMap<BoolParamWidget::BoolParamType, bool> BoolParamWidget::defaultValues
= {{USE_FLASH_ATTENTION, true},

{BOOL_PARAM_COUNT, false}};

BoolParamWidget::BoolParamWidget(QWidget *parent, BoolParamType type)
    : QWidget(parent)
    , ui(new Ui::BoolParamWidget)
{
    ui->setupUi(this);

    this->type = type;
    ui->label->setText(names[type]);
    ui->checkBox->setChecked(defaultValues[type]);
}

BoolParamWidget::~BoolParamWidget()
{
    delete ui;
}

BoolParamWidget::BoolParamType BoolParamWidget::getType()
{
    return this->type;
}

bool BoolParamWidget::getValue()
{
    return ui->checkBox->isChecked();
}
