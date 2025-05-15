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
    llama_context_params ctx_params{};
    llama_context *ctx;
    llama_sampler *sampler;
    llama_batch batch{};

    bool forceStop = false;
    bool generating = false;

    QString name;

    void setSamplers(SamplerArray samplers);
    void resetContext();

public:
    LlamaInterface(const QString &modelPath);
    ~LlamaInterface();

    void startGenerating(const QString &prompt, const SamplerArray &samplers);
    void interruptGeneration();

    bool isGenerating();
    QString getName();
    QString getTemplate();
    QString getEOT();
};

#endif // LLAMAINTERFACE_H
