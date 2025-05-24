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
    explicit NumParamWidget(QWidget *parent = nullptr);
    ~NumParamWidget();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::NumParamWidget *ui;
};

#endif // NUMPARAMWIDGET_H
