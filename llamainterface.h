#ifndef LLAMAINTERFACE_H
#define LLAMAINTERFACE_H

#include <llama.h>
#include <string>

#include "chat.h"

class LlamaInterface
{
    llama_model_params params;
    llama_model *model;
    llama_context_params ctx_params;
    llama_context *ctx;
    llama_sampler *sampler;
    llama_batch batch;

    std::string promptify(Chat &chat);
public:
    LlamaInterface(std::string modelPath);
    ~LlamaInterface();

    void reply(Chat &chat);
};

#endif // LLAMAINTERFACE_H
