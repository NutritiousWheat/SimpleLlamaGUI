#ifndef BOOLPARAMWIDGET_H
#define BOOLPARAMWIDGET_H

#include <QWidget>

namespace Ui {
class BoolParamWidget;
}

class BoolParamWidget : public QWidget
{
    Q_OBJECT

public:
    enum BoolParamE
    {
        USE_FLASH_ATTENTION,

        BOOL_PARAM_COUNT
    };
    explicit BoolParamWidget(QWidget *parent, BoolParamE type);
    ~BoolParamWidget();

    BoolParamE getType();
    bool getValue();

private:
    Ui::BoolParamWidget *ui;

    BoolParamE type;

    void init(QString label, bool defaultValue);
};

#endif // BOOLPARAMWIDGET_H
