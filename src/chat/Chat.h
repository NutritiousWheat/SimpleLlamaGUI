#ifndef CHAT_H
#define CHAT_H

#include "../llm/LlamaThread.h"

class Chat : public QObject
{
    Q_OBJECT

public:
    enum chatStateE
    {
        CHAT_STATE_NOT_LOADED,
        CHAT_STATE_LOADING,
        CHAT_STATE_IDLE,
        CHAT_STATE_GENERATING,

        CHAT_STATE_COUNT
    };

private:
    enum messageRoleE
    {
        MESSAGE_ROLE_USER,
        MESSAGE_ROLE_LLM,
        MESSAGE_ROLE_SYSTEM,

        MESSAGE_ROLE_COUNT
    };
    struct messageT
    {
#warning try figuring out a better way to store llama messages
        std::string content; // needs to be std::string to have easy access to it as a C-string
        messageRoleE role;
    };

public slots:
    void on_messageReceived(QString message, SamplersArrayT samplers); // comes from MainWindow
    void on_interruptReceived();                                       // comes from MainWindow
    void on_tokenGenerated(QString token);                             // comes from LlamaThread
    void on_generationEnd();                                           // comes from LlamaThread
    void on_exceptionOccured(QString errorMsg);
    void on_modelLoading();
    void on_modelLoaded();
    void on_modelUnloaded();

signals:
    void replyStart(const QString &prompt, const SamplersArrayT &samplers); // sent to LlamaThread
    void replyStop();                                                       // sent to LlamaThread
    void appendText();                                                      // sent to MainWindow
    void updateGUI(chatStateE state);
    void exceptionOccured(QString e);

private:
    LlamaThread *llamaThread = nullptr;

    QString systemPrompt;
    QVector<messageT> chat_messages;
    QMutex mutex;

    void appendMessage(const QString &message, messageRoleE role);
    static const char *getRoleStringUI(messageRoleE role);
    static const char *getRoleStringPrompt(messageRoleE role);
    QString promptify();

public:
    Chat();
    explicit Chat(const QString &systemPrompt);
    ~Chat() = default;

    void loadModel(const QString &ggufPath);
    void unloadModel();

    void appendUserMessage(const QString &message);
    void appendLLMMessage(const QString &message);
    void appendSystemMessage(const QString &message);
    void continueMessage(const QString &text);
    QString getString();
    [[nodiscard]] size_t size() const;
    void clear();
};

#endif // CHAT_H
