#ifndef LLAMAINTERFACE_H
#define LLAMAINTERFACE_H

#include <llama-cpp.h>
#include <QObject>

#include "Sampler.h"
#include "SamplerArray.h"

typedef std::unique_ptr<const llama_vocab> llama_vocab_ptr;

QT_BEGIN_NAMESPACE
namespace LLM { // TODO: make namespaces consistent
class LlamaInterface;
}
QT_END_NAMESPACE

class LlamaInterface : public QObject
{
    Q_OBJECT

signals:
    void generationEnd();
    void tokenGenerated(QString token);

private:
    llama_model_params modelParams{};
    llama_context_params ctxParams{};

    llama_model_ptr model;
    llama_vocab_ptr vocab;
    llama_context_ptr ctx;
    llama_sampler_ptr sampler;

    bool forceStop = false;
    bool generating = false;

    QString name;


    QVector<llama_token> tokenize(const QString &prompt);
    void generate(QVector<llama_token> &tokens);
    void setSamplers(SamplerArray samplers);
    void resetContext();

public:
    LlamaInterface(const QString &modelPath, llama_model_params modelParams, llama_context_params ctxParams);
    ~LlamaInterface();

    void startGenerating(const QString &prompt, const SamplerArray &samplers);
    void interruptGeneration();

    bool isGenerating() const;
    QString getName();
    QString getTemplate();
    QString getEOT();
};

#endif // LLAMAINTERFACE_H
