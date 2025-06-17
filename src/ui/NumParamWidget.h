#ifndef NUMPARAMWIDGET_H
#define NUMPARAMWIDGET_H

#include <QWidget>

namespace Ui {
class NumParamWidget;
}

class NumParamWidget : public QWidget
{
    Q_OBJECT

public:
    enum NumParamE
    {
        N_GPU_LAYERS,
        N_CONTEXT_TOKENS,
        N_THREADS,

        NUM_PARAM_COUNT
    };

    explicit NumParamWidget(QWidget *parent, NumParamE type);
    ~NumParamWidget();

    int getValue();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::NumParamWidget *ui;
    NumParamE type;

    void init(const QString &label, int defaultValue, int min, int max, int step = 1);

};

#endif // NUMPARAMWIDGET_H
