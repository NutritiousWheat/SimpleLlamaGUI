#include "LlamaChatMessage.h"

LlamaChatMessage::LlamaChatMessage()
{
    this->content = "";
    this->role = "";
}

LlamaChatMessage::LlamaChatMessage(QByteArray content, QByteArray role)
{
    this->content = content;
    this->role = role;
}

llama_chat_message LlamaChatMessage::getRaw()
{
    return {this->content.constData(), this->role.constData()};
}