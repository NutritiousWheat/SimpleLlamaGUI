#include "NumParamWidget.h"
#include "ui_NumParamWidget.h"

QMap<NumParamWidget::NumParamType, QString> NumParamWidget::names // TODO: use Qt's localization
    = {{N_GPU_LAYERS, "GPU Layers"},
    {N_CONTEXT_TOKENS, "Context Tokens"},
    {N_THREADS, "Threads"},

    {NUM_PARAM_COUNT, "Unknown num param"}};


QMap<NumParamWidget::NumParamType, int> NumParamWidget::defaultValues
    = {{N_GPU_LAYERS, 256},
    {N_CONTEXT_TOKENS, 4096},
    {N_THREADS, 0},

    {NUM_PARAM_COUNT, 0}};

QMap<NumParamWidget::NumParamType, NumParamWidget::NumParamRange> NumParamWidget::ranges
    = {{N_GPU_LAYERS, {0, 256}},
    {N_CONTEXT_TOKENS, {4096, 65536}}, // TODO: get defaulot context from model
    {N_THREADS, {0, 256}},

    {NUM_PARAM_COUNT, {0, 0}}};


QMap<NumParamWidget::NumParamType, int> NumParamWidget::steps
    = {{N_GPU_LAYERS, 1},
    {N_CONTEXT_TOKENS, 512},
    {N_CONTEXT_TOKENS, 0},

    {NUM_PARAM_COUNT, 1}};

NumParamWidget::NumParamWidget(QWidget *parent, NumParamType type)
    : QWidget(parent)
    , ui(new Ui::NumParamWidget)
{
    ui->setupUi(this);

    this->type = type;
    ui->label->setText(names[type]);

    ui->horizontalSlider->setRange(ranges[type].min, ranges[type].max);
    ui->horizontalSlider->setSingleStep(steps[type]);
    ui->horizontalSlider->setValue(defaultValues[type]);

    ui->lineEdit->setText(QString::number(ui->horizontalSlider->value()));
}

NumParamWidget::~NumParamWidget()
{
    delete ui;
}

int NumParamWidget::getValue()
{
    return ui->horizontalSlider->value();
}

void NumParamWidget::on_lineEdit_textChanged(const QString &arg1)
{
    ui->horizontalSlider->setValue(arg1.toInt());
}

void NumParamWidget::on_horizontalSlider_valueChanged(int value)
{
    ui->lineEdit->setText(QString::number(value));
}
