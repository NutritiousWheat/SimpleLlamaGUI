#ifndef CHAT_H
#define CHAT_H

#include <llama.h>
#include <mutex>
#include <string>
#include <vector>

class Chat
{
  private:
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

    std::string systemPrompt;
    std::vector<messageT> messages;
    std::mutex mutex;

  public:
    Chat();
    Chat(std::string systemPrompt);
    ~Chat() = default;

    void appendUserMessage(std::string message);
    void appendLLMMessage(std::string message);
    void appendSystemMessage(std::string message);
    void continueMessage(std::string text);
    llama_chat_message getLlamaMessage(size_t index);
    const std::string getString();
    size_t size();
    void clear();
};

#endif // CHAT_H
