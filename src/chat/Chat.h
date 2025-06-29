#ifndef CHAT_H
#define CHAT_H

#include "../llm/Interface/LlamaThread.h"
#include "ChatMessage.h"

class Chat : public QObject
{
    Q_OBJECT

public:
    enum ChatState {
        CHAT_STATE_NOT_LOADED,
        CHAT_STATE_LOADING,
        CHAT_STATE_IDLE,
        CHAT_STATE_GENERATING,

        CHAT_STATE_COUNT
    };

public slots:
    void on_tokenGenerated(QString token); // comes from LlamaThread
    void on_generationEnd();               // comes from LlamaThread
    void on_exceptionOccured(const QString &errorMsg);
    void on_modelLoading();
    void on_modelLoaded();
    void on_modelUnloaded();

signals:
    void appendText(QString &text); // sent to MainWindow
    void newMessage(const QString &text, ChatMessage::MessageRole role); // sent to MainWindow
    void updateGUI(ChatState state); // sent to MainWindow
    void exceptionOccured(QString e); // sent to MainWindow

private:
    std::unique_ptr<LlamaThread> llamaThread;

    QString systemPrompt;
    QVector<ChatMessage> chatMessages;
    QMutex mutex;

    void appendMessage(const QString &message, ChatMessage::MessageRole role);
    QString promptify(bool newMessage);

public:
    Chat();
    explicit Chat(const QString &systemPrompt);
    ~Chat() = default;

    void loadModel(const QString &modelPath, llama_model_params modelParams, llama_context_params ctxParams);
    void unloadModel();

    void appendUserMessage(const QString &message);
    void appendLLMMessage(const QString &message);
    void appendSystemMessage(const QString &message);

    void reply(const QString &userMessage, const SamplerArray &samplers);
    void continueLastMessage(const SamplerArray &samplers);
    void interruptGeneration();

    [[nodiscard]] size_t size() const;
    void clear();
    [[nodiscard]] QString getModelName() const;
};

#endif // CHAT_H
