#include "chat.h"

Chat::Chat()
{
    this->systemPrompt = "You are a helpful assistant. Assist with whatever user requires.";
    this->appendSystemMessage(this->systemPrompt);
}

Chat::Chat(std::string systemPrompt)
{
    this->systemPrompt = systemPrompt;
    this->appendSystemMessage(this->systemPrompt);
}

void Chat::appendUserMessage(std::string message)
{
    const std::lock_guard<std::mutex> lock(this->mutex);
    this->messages.push_back({message, USER});
}

void Chat::appendLLMMessage(std::string message)
{
    const std::lock_guard<std::mutex> lock(this->mutex);
    this->messages.push_back({message, LLM});
}

void Chat::appendSystemMessage(std::string message)
{
    const std::lock_guard<std::mutex> lock(this->mutex);
    this->messages.push_back({message, SYSTEM});
}

void Chat::continueMessage(std::string text)
{
    const std::lock_guard<std::mutex> lock(this->mutex);
    this->messages[this->messages.size() - 1].text += text;
}

llama_chat_message Chat::getLlamaMessage(size_t index)
{
    llama_chat_message message;

    message.content = this->messages[index].text.c_str();

    switch (this->messages[index].type)
    {
    case USER:
        message.role = "user";
        break;
    case LLM:
        message.role = "assisstant";
        break;
    case SYSTEM:
        message.role = "system";
        break;
    default:
        message.role = "unknown";
        break;
    }

    return message;
}

const std::string Chat::getString()
{
    std::string output;

    for (messageT &message : this->messages)
    {
        switch (message.type)
        {
        case USER:
            output += "USER: ";
            break;
        case LLM:
            output += "LLM: ";
            break;
        case SYSTEM:
            continue;
        default:
            output += "UNKNOWN: ";
            break;
        }

        output += message.text + "\n";
    }

    return output;
}

size_t Chat::size()
{
    return messages.size();
}

void Chat::clear()
{
    this->mutex.lock();
    this->messages.clear();
    this->mutex.unlock();

    this->appendSystemMessage(this->systemPrompt);
}
