#include "NumParamWidget.h"

#include "ui_NumParamWidget.h"

NumParamWidget::NumParamWidget(QWidget *parent, NumParamE type)
    : QWidget(parent)
    , ui(new Ui::NumParamWidget)
{
    ui->setupUi(this);

    this->type = type;

    switch (type) {
    case N_GPU_LAYERS:
        init("GPU Layers", 0, 0, 256); // TODO: use Qt's localization
        break;
    case N_CONTEXT_TOKENS:
        init("Context Tokens", 4096, 512, 65536, 512); // TODO: use actual max limit
        break;
    case N_THREADS:
        init("Threads", 0, 0, 256);
        break;
    default:
        init("Unknown Parameter", 0, 0, 0);
    }
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
    ui->horizontalSlider->setValue(arg1.toInt()); // TODO: maybe respect the step size
}

void NumParamWidget::on_horizontalSlider_valueChanged(int value)
{
    ui->lineEdit->setText(QString::number(value));
}

void NumParamWidget::init(QString label, int defaultValue, int min, int max, int step)
{
    ui->label->setText(label);

    ui->horizontalSlider->setValue(defaultValue);
    ui->horizontalSlider->setRange(min, max);
    ui->horizontalSlider->setSingleStep(step);

    ui->lineEdit->setText(QString::number(ui->horizontalSlider->value()));
}
