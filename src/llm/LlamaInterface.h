#ifndef LLAMAINTERFACE_H
#define LLAMAINTERFACE_H

#include <llama.h>
#include <QObject>

#include "RNG.h"
#include "Sampler.h"
#include "SamplerArray.h"

QT_BEGIN_NAMESPACE
namespace LLM {
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
    RNG rng;

    llama_model_params params{};
    llama_model *model;
    const llama_vocab *vocab;
    llama_context_params ctx_params{};
    llama_context *ctx;
    llama_sampler *sampler;

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

    bool isGenerating();
    QString getName();
    QString getTemplate();
    QString getEOT();
};

#endif // LLAMAINTERFACE_H
