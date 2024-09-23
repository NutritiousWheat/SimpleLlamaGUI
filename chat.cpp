#include "chat.h"

void Chat::appendUserMessage(std::string message)
{
    this->messages.push_back({message, USER});
}

void Chat::appendLLMMessage(std::string message)
{
    this->messages.push_back({message, LLM});
}

void Chat::appendSystemMessage(std::string message)
{
    this->messages.push_back({message, SYSTEM});
}

llama_chat_message Chat::getLlamaMessage(size_t index)
{
    llama_chat_message message;

    message.content = this->messages[index].text.c_str();

    switch (this->messages[index].type) {
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

Chat::messageT &Chat::getMessage(size_t index)
{
    return this->messages[index];
}

const std::string Chat::getString()
{
    std::string output;

    for (messageT &message : this->messages)
    {
        switch (message.type) {
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

