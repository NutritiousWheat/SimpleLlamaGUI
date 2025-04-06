#ifndef SAMPLERSLIDER_H
#define SAMPLERSLIDER_H

#include <QSlider>

#include "../llm/Sampler.h"
#include "MainWindow.h"

class SamplerSlider : public QSlider {
    Q_OBJECT
    Q_PROPERTY(Sampler::SamplerTypeE samplerType)
private slots:
    void on_valueChanged(int value);

    signals:
    void samplerChanged(Sampler sampler);
private:
    const float decimalMultiplier = 100.f;
    Sampler sampler;

    public:
        explicit SamplerSlider(QWidget *parent);
        ~SamplerSlider() = default;

    void init(MainWindow *window, Sampler::SamplerTypeE samplerType); //TODO: this should be done in the constructor
};



#endif //SAMPLERSLIDER_H
