#include "MainWindow.h"

#include "SamplerWidget.h"
#include "ui_SamplerWidget.h"

float SamplerWidget::scaleMultiplier = 100.0f; // TODO: will break for != 2 decimal points

SamplerWidget::SamplerWidget(QWidget *parent, Sampler::Type type)
    : QWidget(parent)
    , ui(new Ui::SamplerWidget)
{
    Sampler::Range samplerRange{};
    int min;
    int max;
    int value;
    QString valueText;

    ui->setupUi(this);

    sampler = Sampler(type);

    ui->samplerLabel->setText(sampler.getName());
    samplerRange = sampler.getRange();

    if (sampler.isInt()) {
        min = static_cast<int>(samplerRange.min);
        max = static_cast<int>(samplerRange.max);
        value = static_cast<int>(sampler.getValue());
        valueText = QString::number(static_cast<int>(sampler.getValue()));
    }
    else {
        min = scaleToSlider(static_cast<float>(samplerRange.min));
        max = scaleToSlider(static_cast<float>(samplerRange.max));
        value = scaleToSlider(static_cast<float>(sampler.getValue()));
        valueText = QString::number(static_cast<float>(sampler.getValue()));
    }

    ui->samplerSlider->setRange(min, max);
    ui->samplerSlider->setValue(value);
    ui->samplerLine->setText(valueText);
}

SamplerWidget::~SamplerWidget()
{
    delete ui;
}

Sampler SamplerWidget::getValue()
{
    return sampler;
}

void SamplerWidget::on_samplerSlider_valueChanged(int value)
{
    if (sampler.isInt()) {
        sampler.setValue(value);
        ui->samplerLine->setText(QString::number(value));
    }
    else {
        sampler.setValue(scaleToSampler(value));
        ui->samplerLine->setText(QString::number(scaleToSampler(value)));
    }
}

void SamplerWidget::on_samplerLine_textChanged(const QString &text)
{
    if (sampler.isInt()) {
        int value = text.toInt();
        sampler.setValue(value);
        ui->samplerSlider->setValue(value);
    } else {
        float value = text.toFloat();
        sampler.setValue(value);
        ui->samplerSlider->setValue(scaleToSlider(value));
    }
}

float SamplerWidget::scaleToSampler(int value)
{
    return static_cast<float>(value) / scaleMultiplier;
}

int SamplerWidget::scaleToSlider(float value)
{
    return static_cast<int>(value * scaleMultiplier);
}