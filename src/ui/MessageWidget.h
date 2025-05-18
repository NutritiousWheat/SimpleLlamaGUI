#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>

#include "../chat/Chat.h"

namespace Ui {
class MessageWidget;
}

class MessageWidget : public QWidget
{
    Q_OBJECT

private:
    Chat::messageRoleE role;

public:
    explicit MessageWidget(QWidget *parent, Chat::messageRoleE role);
    ~MessageWidget();

    void appendText(const QString &text);


private:
    Ui::MessageWidget *ui;
};

#endif // MESSAGEWIDGET_H
