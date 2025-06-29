#include "Chat.h"

#include "LlamaChatMessage.h"
#include <memory>

#include "../ui/MainWindow.h"

Chat::Chat()
{
    // TODO: it is not always needed
    // this->systemPrompt = QString("You are a helpful assistant. Assist with whatever user requires.");
    // this->appendSystemMessage(this->systemPrompt);
}

Chat::Chat(const QString &systemPrompt)
{
    this->systemPrompt = systemPrompt;
    this->appendSystemMessage(this->systemPrompt);
}

void Chat::on_tokenGenerated(QString token)
{
    this->chatMessages.last().appendText(token);
    emit appendText(token);
}

void Chat::on_generationEnd()
{
    emit updateGUI(CHAT_STATE_IDLE);
}

void Chat::on_exceptionOccured(const QString &errorMsg)
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

void Chat::appendMessage(const QString &message, const ChatMessage::MessageRole role)
{
    const std::lock_guard lock(this->mutex);

    chatMessages.append(ChatMessage(role, message));

    emit newMessage(message, role);
}

QString Chat::promptify(bool newMessage)
{
// TODO: bandaid solution with magic number
// TODO: russian seems to break token generation
    QString prompt;
    QVector<LlamaChatMessage> llamaChatMessages(this->size());
    QVector<llama_chat_message> llamaChatMessagesRaw(this->size());
    QByteArray templateCStr = llamaThread->getTemplate().toUtf8();
    char buf[8192];

    this->mutex.lock();

    for (int i = 0; i < this->size(); i++) {
        llamaChatMessages[i] = chatMessages[i].getLlamaChatMessage();
        llamaChatMessagesRaw[i] = llamaChatMessages[i].getRaw(); // TODO: this is ugly, figure out a better solution
    }

    this->mutex.unlock();

    llama_chat_apply_template(templateCStr, llamaChatMessagesRaw.constData(), this->size(), newMessage, buf, 8192);

    prompt = QString::fromUtf8(buf);

    if (!newMessage) {
        prompt.remove(llamaThread->getEOT());
    }

    return prompt;
}

void Chat::loadModel(const QString &modelPath, llama_model_params modelParams, llama_context_params ctxParams)
{
    unloadModel();
    emit updateGUI(CHAT_STATE_LOADING);
    try {
        llamaThread = std::make_unique<LlamaThread>(modelPath, modelParams, ctxParams);
        llamaThread->start();

        connect(llamaThread.get(), &LlamaThread::tokenGenerated, this, &Chat::on_tokenGenerated);
        connect(llamaThread.get(), &LlamaThread::generationEnd, this, &Chat::on_generationEnd);
        connect(llamaThread.get(), &LlamaThread::exceptionOccured, this, &Chat::on_exceptionOccured);
        connect(llamaThread.get(), &LlamaThread::modelLoading, this, &Chat::on_modelLoading);
        connect(llamaThread.get(), &LlamaThread::modelLoaded, this, &Chat::on_modelLoaded);
        connect(llamaThread.get(), &LlamaThread::modelUnloaded, this, &Chat::on_modelUnloaded);

    }
    catch (std::exception &e) {
        emit exceptionOccured(e.what());
        emit updateGUI(CHAT_STATE_NOT_LOADED);
    }
}

void Chat::unloadModel()
{
    if (llamaThread) {
        llamaThread.release();
        llamaThread = nullptr;
    }
    emit updateGUI(CHAT_STATE_NOT_LOADED);
}

void Chat::appendUserMessage(const QString &message)
{
    appendMessage(message, ChatMessage::MESSAGE_ROLE_USER);
}

void Chat::appendLLMMessage(const QString &message)
{
    appendMessage(message, ChatMessage::MESSAGE_ROLE_LLM);
}

void Chat::appendSystemMessage(const QString &message)
{
    appendMessage(message, ChatMessage::MESSAGE_ROLE_SYSTEM);
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

size_t Chat::size() const
{
    return chatMessages.size();
}

void Chat::clear()
{
    this->mutex.lock();
    this->chatMessages.clear();
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
