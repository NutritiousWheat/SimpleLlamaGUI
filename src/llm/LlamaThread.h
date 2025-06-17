#ifndef LLAMATHREAD_H
#define LLAMATHREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QException>

#include "LlamaInterface.h"
#include "SamplerArray.h"

QT_BEGIN_NAMESPACE
namespace LLM {
class LlamaThread;
}
QT_END_NAMESPACE

class LlamaThread : public QThread
{
    Q_OBJECT

    public slots:
    void on_tokenGenerated(const QString &token); // comes from LlamaInterface
    void on_generationEnd(); // comes from LlamaInterface

    signals:
    void replyStart(
            QString prompt, SamplerArray samplers); // proxied from Chat to LlamaInterface
    void replyStop(); // proxied from Chat to LlamaInterface
    void tokenGenerated(QString token); // proxied from LlamaInterface to Chat
    void generationEnd(); // proxied from LlamaInterface to Chat
    void exceptionOccured(QString e);
    void modelLoading();
    void modelLoaded();
    void modelUnloaded();

private:

    LlamaInterface *llama;
    QString modelPath;
    QString name;
    QMutex generationMutex;
    QWaitCondition generationCondition;
    bool loaded;

    QString prompt;
    SamplerArray samplers;
    llama_model_params modelParams{};
    llama_context_params ctxParams{};

    void run() override;

public:
    LlamaThread(const QString &modelPath, llama_model_params modelParams, llama_context_params ctxParams);
    ~LlamaThread();

    void startGenerating(const QString &prompt, const SamplerArray &samplers);
    void interruptGeneration();

    bool isGenerating();
    QString getName();
    QString getTemplate();
    QString getEOT();
};

#endif // LLAMATHREAD_H
