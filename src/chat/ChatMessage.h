#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H
#include <QMap>
#include <QObject>

#include "LlamaChatMessage.h"

class ChatMessage {

public:
    enum MessageRole {
        MESSAGE_ROLE_USER,
        MESSAGE_ROLE_LLM,
        MESSAGE_ROLE_SYSTEM,

        MESSAGE_ROLE_COUNT
    };

private:
    static QMap<MessageRole, QString> roleStringsUI;
    static QMap<MessageRole, QString> roleStringsPrompt;

    QString text;
    MessageRole role;

public:

    static QString getRoleStringUI(MessageRole role);
    static QString getRoleStringPrompt(MessageRole role);

    ChatMessage();
    ChatMessage(MessageRole role);
    ChatMessage(MessageRole role, QString text);
    ~ChatMessage() = default;

    void setText(QString text);
    void appendText(QString text);
    void setRole(MessageRole role);

    QString getText() const;
    MessageRole getRole() const;
    QString getRoleStringUI() const;
    QString getRoleStringPrompt() const;

    LlamaChatMessage getLlamaChatMessage() const;
};



#endif //CHATMESSAGE_H
