#include "ChatMessage.h"

QMap<ChatMessage::MessageRole, QString> ChatMessage::roleStringsUI // TODO: use qt localization
= {
    {MESSAGE_ROLE_USER, "User"},
    {MESSAGE_ROLE_LLM, "LLM"},
    {MESSAGE_ROLE_SYSTEM, "System"},

    {MESSAGE_ROLE_COUNT, "Unknown role"}
};

QMap<ChatMessage::MessageRole, QString> ChatMessage::roleStringsPrompt // TODO: allow user to change those
= {
    {MESSAGE_ROLE_USER, "user"},
    {MESSAGE_ROLE_LLM, "assistant"},
    {MESSAGE_ROLE_SYSTEM, "system"},

    {MESSAGE_ROLE_COUNT, "unknown"}
};

ChatMessage::ChatMessage()
{
    this->role = MESSAGE_ROLE_SYSTEM;
    this->text = "";
}

ChatMessage::ChatMessage(MessageRole role)
{
    this->role = role;
    this->text = "";
}

ChatMessage::ChatMessage(MessageRole role, QString text)
{
    this->role = role;
    this->text = text;
}

void ChatMessage::setText(QString text)
{
    this->text = text;
}

void ChatMessage::appendText(QString text)
{
    this->text += text;
}

void ChatMessage::setRole(MessageRole role)
{
    this->role = role;
}

QString ChatMessage::getText() const
{
    return this->text;
}

ChatMessage::MessageRole ChatMessage::getRole() const
{
    return this->role;
}

QString ChatMessage::getRoleStringUI(MessageRole role)
{
    return roleStringsUI[role];
}

QString ChatMessage::getRoleStringPrompt(MessageRole role)
{
    return roleStringsPrompt[role];
}

QString ChatMessage::getRoleStringUI() const
{
    return getRoleStringUI(this->role);
}

QString ChatMessage::getRoleStringPrompt() const
{
    return getRoleStringPrompt(this->role);
}

LlamaChatMessage ChatMessage::getLlamaChatMessage() const
{
    return LlamaChatMessage(getText().toUtf8(), getRoleStringPrompt().toUtf8());
}
