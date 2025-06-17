#ifndef CHAT_H
#define CHAT_H

#include "../llm/LlamaThread.h"

class Chat : public QObject
{
    Q_OBJECT

public:
    enum chatStateE {
        CHAT_STATE_NOT_LOADED,
        CHAT_STATE_LOADING,
        CHAT_STATE_IDLE,
        CHAT_STATE_GENERATING,

        CHAT_STATE_COUNT
    };

    enum messageRoleE {
        MESSAGE_ROLE_USER,
        MESSAGE_ROLE_LLM,
        MESSAGE_ROLE_SYSTEM,

        MESSAGE_ROLE_COUNT
    };

private:

    struct messageT
    {
        // TODO: try figuring out a better way to store llama messages
        std::string content; // needs to be std::string to have easy access to it as a C-string
        messageRoleE role;
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
    void newMessage(const QString &text, messageRoleE role); // sent to MainWindow
    void updateGUI(chatStateE state); // sent to MainWindow
    void exceptionOccured(QString e); // sent to MainWindow

private:
    LlamaThread *llamaThread = nullptr;

    QString systemPrompt;
    QVector<messageT> chat_messages;
    QMutex mutex;

    void appendMessage(const QString &message, messageRoleE role);
    QString promptify(bool newMessage);

public:
    Chat();
    explicit Chat(const QString &systemPrompt);
    ~Chat() = default;

    static const char *getRoleStringUI(messageRoleE role);
    static const char *getRoleStringPrompt(messageRoleE role);

    void loadModel(const QString &modelPath, llama_model_params modelParams, llama_context_params ctxParams);
    void unloadModel();

    void appendUserMessage(const QString &message);
    void appendLLMMessage(const QString &message);
    void appendSystemMessage(const QString &message);

    void reply(const QString &userMessage, const SamplerArray &samplers);
    void continueLastMessage(const SamplerArray &samplers);
    void interruptGeneration();

    QString getString();
    [[nodiscard]] size_t size() const;
    void clear();
    QString getModelName() const;
};

#endif // CHAT_H
