#include <cmath> // TODO: maybe not?

#include "MessageWidget.h"
#include "ui_MessageWidget.h"

MessageWidget::MessageWidget(QWidget *parent, const ChatMessage::MessageRole role)
    : QWidget(parent)
    , ui(new Ui::MessageWidget)
{
    ui->setupUi(this);

    this->role = role;

    ui->roleLabel->setText(ChatMessage::getRoleStringUI(role));
    if (role == ChatMessage::MESSAGE_ROLE_USER) { // TODO: make it less ugly
        ui->roleLabel->setStyleSheet("QLabel { background-color : rgba(0, 255, 0, 80); color : black; }");
    }
    else if (role == ChatMessage::MESSAGE_ROLE_LLM) {
        ui->roleLabel->setStyleSheet("QLabel { background-color : rgba(255, 0, 0, 80); color : black; }");
    }
}

MessageWidget::~MessageWidget()
{
    delete ui;
}

void MessageWidget::appendText(const QString &text)
{
    ui->messageText->moveCursor(QTextCursor::End);
    ui->messageText->insertPlainText(text);
    ui->messageText->setMinimumHeight(std::ceil(ui->messageText->document()->size().height()));
}
