#ifndef CHAT_H
#define CHAT_H

#include <mutex>
#include <string>
#include <vector>
#include <llama.h>

class Chat
{
    typedef enum
    {
        USER,
        LLM,
        SYSTEM
    } messageTypeE;

    typedef struct
    {
        std::string text;
        messageTypeE type;
    } messageT;

    std::vector<messageT> messages;
    std::mutex mutex;

public:
    Chat() = default;
    ~Chat() = default;

    void appendUserMessage(std::string message);
    void appendLLMMessage(std::string message);
    void appendSystemMessage(std::string message);
    void continueMessage(std::string text);
    llama_chat_message getLlamaMessage(size_t index);
    const std::string getString();
    size_t size();
};

#endif // CHAT_H
