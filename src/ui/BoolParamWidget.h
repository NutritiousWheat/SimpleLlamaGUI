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
    enum BoolParamType
    {
        USE_FLASH_ATTENTION,

        BOOL_PARAM_COUNT
    };
    explicit BoolParamWidget(QWidget *parent, BoolParamType type);
    ~BoolParamWidget();

    BoolParamType getType();
    bool getValue();

private:
    static QMap<BoolParamType, QString> names;
    static QMap<BoolParamType, bool> defaultValues;
    Ui::BoolParamWidget *ui;

    BoolParamType type;
};

#endif // BOOLPARAMWIDGET_H
