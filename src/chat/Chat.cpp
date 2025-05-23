#include "Chat.h"

#include "../ui/MainWindow.h"

Chat::Chat()
{
    // TODO: it is not always needed
    //this->systemPrompt = QString("You are a helpful assistant. Assist with whatever user requires.");
    //this->appendSystemMessage(this->systemPrompt);
}

Chat::Chat(const QString &systemPrompt)
{
    //this->systemPrompt = systemPrompt;
    //this->appendSystemMessage(this->systemPrompt);
}

void Chat::on_tokenGenerated(QString token)
{
    this->chat_messages.last().content += token.toStdString();
    emit appendText(token);
}

void Chat::on_generationEnd()
{
    emit updateGUI(CHAT_STATE_IDLE);
}

void Chat::on_exceptionOccured(QString errorMsg)
{
    emit exceptionOccured(errorMsg);
}

void Chat::on_modelLoading()
{
    emit updateGUI(CHAT_STATE_LOADING);
}

void Chat::on_modelLoaded()
{
    emit updateGUI(CHAT_STATE_IDLE);
}

void Chat::on_modelUnloaded()
{
    emit updateGUI(CHAT_STATE_NOT_LOADED);
}

void Chat::appendMessage(const QString &message, const messageRoleE role)
{
    std::string test = message.toStdString();
    messageT chat_message;

    const std::lock_guard lock(this->mutex);

    chat_message.content = message.toStdString();
    chat_message.role = role;
    chat_messages.append(chat_message);

    emit newMessage(message, role);
}

const char *Chat::getRoleStringUI(const messageRoleE role)
{
    switch (role) {
    case MESSAGE_ROLE_USER:
        return "User";
    case MESSAGE_ROLE_LLM:
        return "LLM";
    case MESSAGE_ROLE_SYSTEM:
        return "System";
    default:
        return "Unknown";
    }
}

const char *Chat::getRoleStringPrompt(const messageRoleE role)
{
    switch (role) {
    case MESSAGE_ROLE_USER:
        return "user";
    case MESSAGE_ROLE_LLM:
        return "assistant";
    case MESSAGE_ROLE_SYSTEM:
        return "system";
    default:
        return "unknown";
    }
}


QString Chat::promptify(bool newMessage)
{
// TODO: bandaid solution with magic number
// TODO: russian seems to break token generation
    QString prompt;
    llama_chat_message *llama_messages;
    char buf[8192];
    std::string templateStr;
    const char *templateCStr;

    this->mutex.lock();

    llama_messages = new llama_chat_message[this->size()];

    for (int i = 0; i < this->size(); i++) {
        llama_messages[i].content = this->chat_messages[i].content.c_str();
        llama_messages[i].role = getRoleStringPrompt(this->chat_messages[i].role);
    }

    this->mutex.unlock();

    templateStr = llamaThread->getTemplate().toStdString();
    templateCStr = templateStr.c_str();

    llama_chat_apply_template(templateCStr, llama_messages, this->size(), newMessage, buf, 8192);

    delete[] llama_messages;

    prompt = QString::fromUtf8(buf);

    if (!newMessage) {
        prompt.remove(llamaThread->getEOT()); // TODO: there may be better ways to create a prompt without the EOT token
    }

    return prompt;
}

void Chat::loadModel(const QString &modelPath)
{
    unloadModel();
    emit updateGUI(CHAT_STATE_LOADING);
    try {
        llamaThread = new LlamaThread(modelPath);
        llamaThread->start();

        connect(llamaThread, &LlamaThread::tokenGenerated, this, &Chat::on_tokenGenerated);
        connect(llamaThread, &LlamaThread::generationEnd, this, &Chat::on_generationEnd);
        connect(llamaThread, &LlamaThread::exceptionOccured, this, &Chat::on_exceptionOccured);
        connect(llamaThread, &LlamaThread::modelLoading, this, &Chat::on_modelLoading);
        connect(llamaThread, &LlamaThread::modelLoaded, this, &Chat::on_modelLoaded);
        connect(llamaThread, &LlamaThread::modelUnloaded, this, &Chat::on_modelUnloaded);

    }
    catch (std::exception &e) {
        emit exceptionOccured(e.what());
        emit updateGUI(CHAT_STATE_NOT_LOADED);
    }
}

void Chat::unloadModel()
{
    if (llamaThread) {
        llamaThread->quit();
        delete llamaThread;
        llamaThread = nullptr;
    }
    emit updateGUI(CHAT_STATE_NOT_LOADED);
}

void Chat::appendUserMessage(const QString &message)
{
    appendMessage(message, MESSAGE_ROLE_USER);
}

void Chat::appendLLMMessage(const QString &message)
{
    appendMessage(message, MESSAGE_ROLE_LLM);
}

void Chat::appendSystemMessage(const QString &message)
{
    appendMessage(message, MESSAGE_ROLE_SYSTEM);
}

void Chat::reply(const QString &userMessage, const SamplerArray &samplers)
{
    QString prompt;

    this->appendUserMessage(userMessage);
    prompt = this->promptify(true);

    this->appendLLMMessage("");

    emit updateGUI(CHAT_STATE_GENERATING);

    llamaThread->startGenerating(prompt, samplers);
}

void Chat::interruptGeneration()
{
    llamaThread->interruptGeneration();
}

void Chat::continueLastMessage(const SamplerArray &samplers)
{
    QString prompt;

    prompt = this->promptify(false);

    emit updateGUI(CHAT_STATE_GENERATING);
    llamaThread->startGenerating(prompt, samplers);
}

QString Chat::getString()
{
    QString output;

    for (messageT &message : this->chat_messages) {
        if (message.role != MESSAGE_ROLE_SYSTEM) {
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

QString Chat::getModelName() const
{
    if (llamaThread) {
        return llamaThread->getName();
    }

    return "";
}
