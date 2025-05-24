#include "LlamaInterface.h"

#include "../../submodules/llama.cpp/src/llama-model.h" // TODO: why?
#include "SamplerArray.h"
#include <stdexcept>

#define N_PREDICT 250
#define N_CTX 8192
#define N_BATCH 2048
#define N_UBATCH 512

#define THREADS 8

LlamaInterface::LlamaInterface(const QString &modelPath)
{
    llama_backend_init();
    llama_numa_init(GGML_NUMA_STRATEGY_DISABLED);

    params.n_gpu_layers = 256;
    params.split_mode = LLAMA_SPLIT_MODE_LAYER;
    params.main_gpu = 0;
    params.tensor_split = nullptr;
    params.progress_callback = nullptr;
    params.progress_callback_user_data = nullptr;
    params.kv_overrides = nullptr;
    params.vocab_only = false;
    params.use_mmap = false;
    params.use_mlock = false;
    params.check_tensors = false;

    model = llama_model_load_from_file(modelPath.toUtf8(), params);

    if (model == nullptr) {
        throw std::runtime_error("unable to load model");
    }

    ctx_params.n_ctx = N_CTX;     // text context, 0 = from model
    ctx_params.n_batch = N_BATCH; // logical maximum batch size that can be submitted to llama_decode
    ctx_params.n_ubatch = N_UBATCH; // physical maximum batch size

    ctx_params.n_seq_max = 1; // max number of sequences (i.e. distinct states for recurrent models)
    ctx_params.n_threads = params.n_gpu_layers ? 1: THREADS;       // number of threads to use for generation
    ctx_params.n_threads_batch = params.n_gpu_layers ? 1 : THREADS; // number of threads to use for batch processing
    ctx_params.rope_scaling_type
        = LLAMA_ROPE_SCALING_TYPE_UNSPECIFIED; // RoPE scaling type, from `enum llama_rope_scaling_type`
    ctx_params.pooling_type
        = LLAMA_POOLING_TYPE_UNSPECIFIED; // whether to pool (sum) embedding results by sequence id
    ctx_params.attention_type
        = LLAMA_ATTENTION_TYPE_UNSPECIFIED; // attention type to use for embeddings
    ctx_params.rope_freq_base = 0;          // RoPE base frequency, 0 = from model
    ctx_params.rope_freq_scale = 0;         // RoPE frequency scaling factor, 0 = from model
    ctx_params.yarn_ext_factor = -1;        // YaRN extrapolation mix factor, negative = from model
    ctx_params.yarn_attn_factor = -1;       // YaRN magnitude scaling factor
    ctx_params.yarn_beta_fast = -1;         // YaRN low correction dim
    ctx_params.yarn_beta_slow = -1;         // YaRN high correction dim
    ctx_params.yarn_orig_ctx = -1;          // YaRN original context size
    ctx_params.defrag_thold
        = -1; // defragment the KV cache if holes/size > thold, < 0 disabled (default)
    ctx_params.cb_eval = nullptr;
    ctx_params.cb_eval_user_data = nullptr;
    ctx_params.type_k = GGML_TYPE_F16; // data type for K cache [EXPERIMENTAL]
    ctx_params.type_v = GGML_TYPE_F16; // data type for V cache [EXPERIMENTAL]
    ctx_params.embeddings = false;  // if true, extract embeddings (together with logits)
    ctx_params.offload_kqv = false; // whether to offload the KQV ops (including the KV cache) to GPU
    ctx_params.flash_attn = false;  // whether to use flash attention [EXPERIMENTAL]
    ctx_params.abort_callback = nullptr;
    ctx_params.abort_callback_data = nullptr;

    // ctx = llama_init_from_model(model, ctx_params); // TODO: this only needed if i don't reset context
    //
    // if (ctx == nullptr) {
    //     throw std::runtime_error("unable to create context");
    // }

    vocab = llama_model_get_vocab(model);

    sampler = NULL;
    ctx = NULL;

    this->name = QString::fromStdString(model->name);
}

LlamaInterface::~LlamaInterface()
{
    if (sampler)
        llama_sampler_free(sampler);

    if (ctx)
        llama_free(ctx);

    if (model)
        llama_model_free(model);

    llama_backend_free();
}

QVector<llama_token> LlamaInterface::tokenize(const QString &prompt)
{
    int tokenCount;
    char promptCStr[prompt.length() + 1];{};

    llama_token tokensCArr[prompt.length() + 1]{};
    QVector<llama_token> tokens;

    strcpy(promptCStr, prompt.toUtf8().data());

    tokenCount = llama_tokenize(vocab, promptCStr, strlen(promptCStr), tokensCArr, strlen(promptCStr), true, true);

    for (int i = 0; i < tokenCount; i++) {
        tokens.append(tokensCArr[i]);
    }

    return tokens;
}

using namespace std::chrono;

void LlamaInterface::generate(QVector<llama_token> &tokens)
{
    // TODO: do some abstraction over that
#ifdef __APPLE__
    time_point<steady_clock> start;
    time_point<steady_clock> end;
#else
    time_point<system_clock> start;
    time_point<system_clock> end;
#endif
    unsigned long timeMs;

    int pieceSize;
    int tokensGenerated;
    bool firstDecode;

    llama_batch batch;
    llama_token newTokenId;
    char pieceBuffer[256] = {0};

    int tokensPerBatch;

    start = high_resolution_clock::now();
    for (int i = 0; i < tokens.size();) {
        tokensPerBatch = std::min((int) llama_n_ubatch(ctx), tokens.size() - i);
        batch = llama_batch_get_one(&tokens[i], tokensPerBatch);;

        if (llama_decode(ctx, batch) != 0) {
            throw std::runtime_error("first decode failed");
        }

        i += tokensPerBatch;
    }
    end = high_resolution_clock::now();

    timeMs = duration_cast<milliseconds>(end - start).count();

    qDebug("prompt: %d tokens", tokens.size());
    qDebug("prompt processing time: %lu ms, (%f t/s)",
        timeMs,
        (static_cast<double>(tokens.size()) / static_cast<double>(timeMs)) * 1000.0
        );

    tokensGenerated = 0;
    firstDecode = true;

    start = high_resolution_clock::now();
    while (tokensGenerated <= N_PREDICT) {
        if (firstDecode) {
            firstDecode = false;
        }
        else {
            batch = llama_batch_get_one(&newTokenId, 1);

            if (llama_decode(ctx, batch)) {
                throw std::runtime_error("decode failed");
            }
        }

        newTokenId = llama_sampler_sample(sampler, ctx, -1);

        // is it the end?
        if (llama_vocab_is_eog(vocab, newTokenId) || this->forceStop) {
            this->forceStop = false;
            break;
        }

        tokensGenerated += 1;

        pieceSize = llama_token_to_piece(vocab, newTokenId, pieceBuffer, sizeof(pieceBuffer), 0, false);
        pieceBuffer[pieceSize] = '\0'; // llama_token_to_piece does not null-terminate

        emit tokenGenerated(QString(pieceBuffer));
    }
    end = high_resolution_clock::now();

    timeMs = duration_cast<milliseconds>(end - start).count();

    qDebug("generated: %d tokens", tokensGenerated);
    qDebug("generating time: %lu ms, (%f t/s)",
        timeMs,
        (static_cast<double>(tokensGenerated) / static_cast<double>(timeMs)) * 1000.0
        );
}

void LlamaInterface::startGenerating(const QString &prompt, const SamplerArray &samplers)
{
    QVector<llama_token> tokens;

    int n_ctx;
    int n_kv_req;

    generating = true;

    resetContext(); // TODO: bad for performance, do caching instead

    tokens = tokenize(prompt);

    n_ctx = llama_n_ctx(ctx);
    n_kv_req = N_CTX; // TODO: smarter kv cache allocation... and also caching

    if (n_kv_req > n_ctx) {
        throw std::runtime_error("kv cache size is not big enough");
    }

    setSamplers(samplers);

    generate(tokens);

    generating = false;
    forceStop = false;

    emit generationEnd();
}

void LlamaInterface::interruptGeneration()
{
    forceStop = true;
}

bool LlamaInterface::isGenerating()
{
    return generating;
}

void LlamaInterface::setSamplers(SamplerArray samplers)
{
    if (sampler)
        llama_sampler_free(sampler);

    sampler = llama_sampler_chain_init({true}); // no_perf = true

    samplers[Sampler::SAMPLER_TEMP];

    if (sampler == nullptr) {
        throw std::runtime_error("unable to init sampler chain");
    }

    qDebug(
        "Set samplers: temp: %f, top p: %f, min p: %f, top k: %u",
        samplers[Sampler::SAMPLER_TEMP].getValueFloat(),
        samplers[Sampler::SAMPLER_TOP_P].getValueFloat(),
        samplers[Sampler::SAMPLER_MIN_P].getValueFloat(),
        samplers[Sampler::SAMPLER_TOP_K].getValueInt());

    llama_sampler_chain_add(sampler, llama_sampler_init_temp(samplers[Sampler::SAMPLER_TEMP].getValueFloat()));
    llama_sampler_chain_add(sampler, llama_sampler_init_top_p(samplers[Sampler::SAMPLER_TOP_P].getValueFloat(), 1));
    llama_sampler_chain_add(sampler, llama_sampler_init_min_p(samplers[Sampler::SAMPLER_MIN_P].getValueFloat(), 1));
    llama_sampler_chain_add(sampler, llama_sampler_init_top_k(samplers[Sampler::SAMPLER_TOP_K].getValueInt()));

    llama_sampler_chain_add(sampler, llama_sampler_init_dist(rng.getRand()));
}

void LlamaInterface::resetContext()
{
    if (ctx)
        llama_free(ctx);

    ctx = llama_init_from_model(model, ctx_params);

    if (ctx == nullptr) {
        throw std::runtime_error("unable to create context");
    }
}

QString LlamaInterface::getName()
{
    return this->name;
}

QString LlamaInterface::getTemplate()
{
    const char *templateCStr = llama_model_chat_template(model, NULL);

    return {templateCStr};
}

QString LlamaInterface::getEOT()
{
    char buffer[1024]; // TODO: magic number
    llama_token eot = llama_vocab_eot(&model->vocab);

    llama_token_to_piece(&model->vocab, eot, buffer, sizeof(buffer), 0, true);

    return {buffer};
}
