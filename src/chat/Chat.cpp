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
    QString prompt;

    this->appendMessage(message, USER);
    prompt = this->promptify();

    this->appendMessage("", LLM);
#warning send required state here
    emit updateGUI();
    emit replyStart(prompt, samplers);
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

    const std::lock_guard lock(this->mutex);

    chat_message.content = message.toStdString();
    chat_message.role = role;
    chat_messages.append(chat_message);
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


QString Chat::promptify()
{
#warning bandaid solution with magic number
    QString prompt;
    llama_chat_message *llama_messages;
    char buf[8192];
    QString templateStr;
    char *templateCStr;

    this->mutex.lock();

    llama_messages = new llama_chat_message[this->size()];

    for (int i = 0; i < this->size(); i++) {
        llama_messages[i].content = this->chat_messages[i].content.c_str();
        llama_messages[i].role = getRoleStringPrompt(this->chat_messages[i].role);
    }

    this->mutex.unlock();

    templateStr = llamaThread->getTemplate();
    templateCStr = templateStr.toUtf8().data();

    llama_chat_apply_template(templateCStr, llama_messages, this->size(), true, buf, 8192);

    delete[] llama_messages;

    prompt = QString(buf);

    return prompt;
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
    return chat_messages.size();
}

void Chat::clear()
{
    this->mutex.lock();
    this->chat_messages.clear();
    this->mutex.unlock();

    this->appendSystemMessage(this->systemPrompt);
}
