#include "Chat.h"

#include "../ui/MainWindow.h"

Chat::Chat()
{
    this->systemPrompt = QString("You are a helpful assistant. Assist with whatever user requires.");
    this->appendSystemMessage(this->systemPrompt);
}

Chat::Chat(const QString &systemPrompt)
{
    this->systemPrompt = systemPrompt;
    this->appendSystemMessage(this->systemPrompt);
}

void Chat::on_messageReceived(QString message, SamplersArrayT samplers)
{
    this->appendMessage(message, USER);
    this->appendMessage("", LLM);
#warning send required state here
    emit updateGUI();
    emit replyStart(this->llama_messages, samplers);
}

void Chat::on_interruptReceived()
{
    emit replyStop();
}

void Chat::on_tokenGenerated(QString token)
{
    this->chat_messages.last().content += token.toStdString();
    emit appendText();
}

void Chat::on_generationEnd()
{
#warning send required state here
    emit updateGUI();
}

void Chat::appendMessage(const QString &message, const messageRoleE role)
{
    std::string test = message.toStdString();
    messageT chat_message;
    llama_chat_message llama_message;

    const std::lock_guard lock(this->mutex);

    chat_message.content = message.toStdString();
    chat_message.role = role;
    chat_messages.append(chat_message);

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

void Chat::loadModel(const QString &modelPath)
{
    unloadModel();
    llamaThread = new LlamaThread(modelPath);
    llamaThread->start();
    connect(this, &Chat::replyStart, llamaThread, &LlamaThread::on_replyStart);
    connect(this, &Chat::replyStop, llamaThread, &LlamaThread::on_replyStop);
    connect(llamaThread, &LlamaThread::tokenGenerated, this, &Chat::on_tokenGenerated);
    connect(llamaThread, &LlamaThread::generationEnd, this, &Chat::on_generationEnd);


}

void Chat::unloadModel()
{
    if (llamaThread) {
        llamaThread->quit();
        delete llamaThread;
    }
}

void Chat::appendUserMessage(const QString &message)
{
    appendMessage(message, USER);
}

void Chat::appendLLMMessage(const QString &message)
{
    appendMessage(message, LLM);
}

void Chat::appendSystemMessage(const QString &message)
{
    appendMessage(message, SYSTEM);
}

void Chat::continueMessage(const QString &text)
{
#warning not implemented
}

QString Chat::getString()
{
    QString output;

    for (messageT &message : this->chat_messages) {
        if (message.role != SYSTEM) {
            output += getRoleStringUI(message.role);
            output += QString::fromStdString(": " + message.content + "\n");
        }

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
