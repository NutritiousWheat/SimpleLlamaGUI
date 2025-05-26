#include "SamplerWidget.h"

#include "MainWindow.h"
#include "ui_SamplerWidget.h"
#include <qtextstream.h>

SamplerWidget::SamplerWidget(QWidget *parent, Sampler::SamplerTypeE type)
    : QWidget(parent)
    , ui(new Ui::SamplerWidget)
{
    Sampler::SamplerRangeIntT sliderRange;
    ui->setupUi(this);

    sampler = Sampler(type);

    ui->samplerLabel->setText(Sampler::samplerTypeToString(sampler.getType()));

    sliderRange = sampler.getRangeInt();
    ui->samplerSlider->setRange(sliderRange.min, sliderRange.max);
    ui->samplerSlider->setValue(sampler.getValueInt());

    if (sampler.isInt()) {
        ui->samplerLine->setText(QString::number(sampler.getValueInt()));
    }
    else {
        ui->samplerLine->setText(QString::number(sampler.getValueFloat()));
    }
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
        sampler.setValueInt(value);
        ui->samplerLine->setText(QString::number(value));
    }
    else {
        sampler.setValueInt(value);
        ui->samplerLine->setText(QString::number(sampler.getValueFloat()));
    }
}

void SamplerWidget::on_samplerLine_textChanged(const QString &text)
{
    if (sampler.isInt()) {
        int value = text.toInt();
        sampler.setValueInt(value);
        ui->samplerSlider->setValue(value);
    } else {
        float value = text.toFloat();
        sampler.setValueFloat(value);
        ui->samplerSlider->setValue(sampler.getValueInt());
    }
}
