#ifndef LLAMACHATMESSAGE_H
#define LLAMACHATMESSAGE_H
#include <QByteArray>
#include "llama.h"
#include <memory>

class LlamaChatMessage {
    QByteArray content;
    QByteArray role;

public:
    LlamaChatMessage();
    LlamaChatMessage(QByteArray content, QByteArray role);
    ~LlamaChatMessage() = default;

    llama_chat_message getRaw();
};



#endif //LLAMACHATMESSAGE_H
