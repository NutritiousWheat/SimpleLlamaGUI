#ifndef LLAMAINTERFACE_H
#define LLAMAINTERFACE_H

#include <llama.h>
#include <string>

#include "../chat/chat.h"

typedef enum
{
    TEMP,
    TOP_K,
    TOP_P,
    MIN_P,

    SAMPLER_COUNT
} SamplerType;

typedef struct
{
    union {
        float floatValue;
        int32_t intValue;
    } value;
    SamplerType type;
} Sampler;

class LlamaInterface
{
  private:
    llama_model_params params;
    llama_model *model;
    llama_context_params ctx_params;
    llama_context *ctx;
    llama_sampler *sampler;
    llama_batch batch;

    bool forceStop = false;

    std::string promptify(Chat &chat);

  public:
    LlamaInterface(std::string modelPath);
    ~LlamaInterface();

    void reply(Chat &chat);
    void stop();
    void updateSamplers(Sampler samplers[SAMPLER_COUNT]);
};

#endif // LLAMAINTERFACE_H
