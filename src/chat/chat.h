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
    } messageRoleE;

    typedef struct
    {
        std::string content;
        messageRoleE role;
    } message_t;

    std::string systemPrompt;
    std::vector<message_t> chat_messages;
    std::vector<llama_chat_message> llama_messages;
    std::mutex mutex;

    void appendMessage(const std::string& message, messageRoleE role);
    static const char *getRoleStringUI(messageRoleE role);
    static const char *getRoleStringPrompt(messageRoleE role);


  public:
    Chat();
    Chat(std::string systemPrompt);
    ~Chat() = default;

    void appendUserMessage(const std::string& message);
    void appendLLMMessage(const std::string& message);
    void appendSystemMessage(const std::string& message);
    void continueMessage(const std::string& text);
    llama_chat_message getMessage(size_t index) const;
    size_t getAllMessages(llama_chat_message **message_ptr);
    std::string getString();
    size_t size() const;
    void clear();
};

#endif // CHAT_H
