#include "chat.h"

#include <utility>

Chat::Chat()
{
    this->systemPrompt = "You are a helpful assistant. Assist with whatever user requires.";
    this->appendSystemMessage(this->systemPrompt);
}

Chat::Chat(std::string systemPrompt)
{
    this->systemPrompt = std::move(systemPrompt);
    this->appendSystemMessage(this->systemPrompt);
}

void Chat::appendMessage(const std::string &message, const messageRoleE role)
{
    message_t chat_message;
    llama_chat_message llama_message;

    const std::lock_guard<std::mutex> lock(this->mutex);

    chat_message.content = message;
    chat_message.role = role;
    chat_messages.emplace_back(chat_message);

    llama_message.role = getRoleStringPrompt(role);
    llama_message.content = (chat_messages.end() - 1)->content.c_str();
    llama_messages.push_back(llama_message);
}

const char *Chat::getRoleStringUI(const messageRoleE role)
{
    switch (role) {
    case USER:
        return "User";
    case LLM:
        return "LLM";
    case SYSTEM:
        return "System";
    default:
        return "Unknown";
    }
}

const char *Chat::getRoleStringPrompt(const messageRoleE role)
{
    switch (role) {
    case USER:
        return "user";
    case LLM:
        return "assistant";
    case SYSTEM:
        return "system";
    default:
        return "unknown";
    }
}

void Chat::appendUserMessage(const std::string &message)
{
    appendMessage(message, USER);
}

void Chat::appendLLMMessage(const std::string &message)
{
    appendMessage(message, LLM);
}

void Chat::appendSystemMessage(const std::string &message)
{
    appendMessage(message, SYSTEM);
}

void Chat::continueMessage(const std::string &text)
{
    const std::lock_guard<std::mutex> lock(this->mutex);
    this->chat_messages.back().content += text;
    this->llama_messages.back().content = this->chat_messages.back().content.c_str();
}

llama_chat_message Chat::getMessage(const size_t index) const
{
    return this->llama_messages[index];
}

size_t Chat::getAllMessages(llama_chat_message **message_ptr)
{
    const std::lock_guard<std::mutex> lock(this->mutex);
    *message_ptr = new llama_chat_message[this->llama_messages.size()];
    std::copy(this->llama_messages.begin(), this->llama_messages.end(), *message_ptr);
    return this->llama_messages.size();
}

std::string Chat::getString()
{
    std::string output;

    for (message_t &message : this->chat_messages) {
        if (message.role != SYSTEM)
            output += std::string(getRoleStringUI(message.role)) + ": " + message.content + "\n";
    }

    return output;
}

size_t Chat::size() const
{
    return llama_messages.size();
}

void Chat::clear()
{
    this->mutex.lock();
    this->chat_messages.clear();
    this->llama_messages.clear();
    this->mutex.unlock();

    this->appendSystemMessage(this->systemPrompt);
}
