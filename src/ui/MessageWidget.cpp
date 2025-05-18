#include "MessageWidget.h"
#include "ui_MessageWidget.h"

MessageWidget::MessageWidget(QWidget *parent, const Chat::messageRoleE role)
    : QWidget(parent)
    , ui(new Ui::MessageWidget)
{
    ui->setupUi(this);

    this->role = role;

    this->ui->roleLabel->setText(Chat::getRoleStringUI(role));
}

MessageWidget::~MessageWidget()
{
    delete ui;
}

void MessageWidget::appendText(const QString &text)
{
    this->ui->messageText->moveCursor(QTextCursor::End);
    this->ui->messageText->insertPlainText(text);
}
