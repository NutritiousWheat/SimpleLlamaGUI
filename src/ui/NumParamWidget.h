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
    enum NumParamType
    {
        N_GPU_LAYERS,
        N_CONTEXT_TOKENS,
        N_THREADS,

        NUM_PARAM_COUNT
    };

    struct NumParamRange
    {
        int min;
        int max;
    };

    explicit NumParamWidget(QWidget *parent, NumParamType type);
    ~NumParamWidget();

    int getValue();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_horizontalSlider_valueChanged(int value);

private:
    static QMap<NumParamType, QString> names;
    static QMap<NumParamType, int> defaultValues;
    static QMap<NumParamType, NumParamRange> ranges;
    static QMap<NumParamType, int> steps;




    Ui::NumParamWidget *ui;
    NumParamType type;

};

#endif // NUMPARAMWIDGET_H
