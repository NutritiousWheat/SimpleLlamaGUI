#ifndef LLAMATHREAD_H
#define LLAMATHREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QException>

#include "LlamaInterface.h"



QT_BEGIN_NAMESPACE
namespace LLM {
class LlamaThread;
}
QT_END_NAMESPACE

class LlamaThread : public QThread
{
    Q_OBJECT

    public slots:
    void on_replyStart(QString prompt,
                           SamplersArrayT samplers); // comes from Chat
    void on_replyStop(); // comes from Chat
    void on_tokenGenerated(QString token); // comes from LlamaInterface
    void on_generationEnd(); // comes from LlamaInterface

    signals:
    void replyStart(
            QString prompt, SamplersArrayT samplers); // proxied from Chat to LlamaInterface
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
    SamplersArrayT samplers;

    void run() override;

public:
    LlamaThread(const QString &modelPath);
    ~LlamaThread();

    bool isGenerating();
    QString getName();
    QString getTemplate();
};

#endif // LLAMATHREAD_H
