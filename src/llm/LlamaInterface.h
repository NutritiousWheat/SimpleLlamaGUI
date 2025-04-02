#ifndef LLAMAINTERFACE_H
#define LLAMAINTERFACE_H

#include "RNG.h"
#include <llama-sampling.h>
#include <llama.h>
#include <QObject>

QT_BEGIN_NAMESPACE
namespace LLM {
class LlamaInterface;
}
QT_END_NAMESPACE


enum SamplerTypeE {
    TEMP,
    TOP_K,
    TOP_P,
    MIN_P,

    SAMPLER_COUNT
};

struct SamplerT
{
#warning use different types for int and float samplers
    union {
        float floatValue;
        int32_t intValue;
    } value;
    SamplerTypeE type;
};

struct SamplersArrayT
{
    SamplerT array[SAMPLER_COUNT];
};

class LlamaInterface : public QObject
{
    Q_OBJECT

public slots:
    void on_replyStart(const QVector <llama_chat_message> &messages, const SamplersArrayT &samplers);
    void on_replyStop();

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

    QString promptify(const QVector <llama_chat_message> &messages);
    void setSamplers(const SamplersArrayT &samplers);

public:
    LlamaInterface(const QString &modelPath);
    ~LlamaInterface();

    bool isGenerating();
    QString getName();
};

#endif // LLAMAINTERFACE_H
