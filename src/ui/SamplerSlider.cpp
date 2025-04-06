#include "SamplerSlider.h"

SamplerSlider::SamplerSlider(QWidget *parent)
{
    connect(this, &SamplerSlider::valueChanged, this, &SamplerSlider::on_valueChanged);
}

void SamplerSlider::on_valueChanged(int value)
{
    if (sampler.isInt()) {
        sampler.setValueInt(value);
    }
    else {
        sampler.setValueFloat(value / decimalMultiplier);
    }
    emit samplerChanged(sampler);
}

void SamplerSlider::init(MainWindow *window, Sampler::SamplerTypeE samplerType)
{
    this->sampler = Sampler(samplerType);
    connect(this, &SamplerSlider::samplerChanged, window, &MainWindow::on_sampler_valueChanged);
}
