#ifndef SAMPLERWIDGET_H
#define SAMPLERWIDGET_H

#include <QWidget>
#include "../llm/Samplers/Sampler.h"

namespace Ui {
class SamplerWidget;
}

class SamplerWidget : public QWidget
{
    Q_OBJECT
    Sampler sampler;

public:
    explicit SamplerWidget(QWidget *parent, Sampler::SamplerType type);
    ~SamplerWidget();

    Sampler getValue();

private slots:
    void on_samplerSlider_valueChanged(int value);

    void on_samplerLine_textChanged(const QString &text);

private:
    Ui::SamplerWidget *ui;
    static float scaleMultiplier;

    static float scaleToSampler(int value);
    static int scaleToSlider(float value);

};

#endif // SAMPLERWIDGET_H
