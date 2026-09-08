#include "LlamaInterface.h"

#include "../../submodules/llama.cpp/src/llama-model.h" // TODO: why?
#include "../Samplers/SamplerArray.h"
#include <stdexcept>

#define N_PREDICT 250
#define N_BATCH 2048
#define N_UBATCH 512

#define THREADS 8

LlamaInterface::LlamaInterface(const QString &modelPath, llama_model_params modelParams, llama_context_params ctxParams)
{
    llama_backend_init();
    llama_numa_init(GGML_NUMA_STRATEGY_DISABLED);

    this->modelParams.n_gpu_layers = modelParams.n_gpu_layers; // TODO: set parameters outside
    this->modelParams.split_mode = LLAMA_SPLIT_MODE_LAYER;
    this->modelParams.main_gpu = 0;
    this->modelParams.tensor_split = nullptr;
    this->modelParams.progress_callback = nullptr;
    this->modelParams.progress_callback_user_data = nullptr;
    this->modelParams.kv_overrides = nullptr;
    this->modelParams.vocab_only = false;
    this->modelParams.use_mmap = false;
    this->modelParams.use_mlock = false;
    this->modelParams.check_tensors = false;

    model = llama_model_ptr(llama_model_load_from_file(modelPath.toUtf8(), this->modelParams));

    if (model == nullptr) {
        throw std::runtime_error("unable to load model"); // TODO: do proper exceptions
    }

    this->ctxParams.n_ctx = ctxParams.n_ctx;     // text context, 0 = from model // TODO: separate context settings
    this->ctxParams.n_batch = N_BATCH; // logical maximum batch size that can be submitted to llama_decode
    this->ctxParams.n_ubatch = N_UBATCH; // physical maximum batch size

    this->ctxParams.n_seq_max = 1; // max number of sequences (i.e. distinct states for recurrent models)
    this->ctxParams.n_threads = modelParams.n_gpu_layers ? 1: ctxParams.n_threads;       // number of threads to use for generation
    this->ctxParams.n_threads_batch = modelParams.n_gpu_layers ? 1 : ctxParams.n_threads_batch; // number of threads to use for batch processing
    this->ctxParams.rope_scaling_type
        = LLAMA_ROPE_SCALING_TYPE_UNSPECIFIED; // RoPE scaling type, from `enum llama_rope_scaling_type`
    this->ctxParams.pooling_type
        = LLAMA_POOLING_TYPE_UNSPECIFIED; // whether to pool (sum) embedding results by sequence id
    this->ctxParams.attention_type
        = LLAMA_ATTENTION_TYPE_UNSPECIFIED; // attention type to use for embeddings
    this->ctxParams.rope_freq_base = 0;          // RoPE base frequency, 0 = from model
    this->ctxParams.rope_freq_scale = 0;         // RoPE frequency scaling factor, 0 = from model
    this->ctxParams.yarn_ext_factor = -1;        // YaRN extrapolation mix factor, negative = from model
    this->ctxParams.yarn_attn_factor = -1;       // YaRN magnitude scaling factor
    this->ctxParams.yarn_beta_fast = -1;         // YaRN low correction dim
    this->ctxParams.yarn_beta_slow = -1;         // YaRN high correction dim
    this->ctxParams.yarn_orig_ctx = -1;          // YaRN original context size
    this->ctxParams.defrag_thold
        = -1; // defragment the KV cache if holes/size > thold, < 0 disabled (default)
    this->ctxParams.cb_eval = nullptr;
    this->ctxParams.cb_eval_user_data = nullptr;
    this->ctxParams.type_k = GGML_TYPE_F16; // data type for K cache [EXPERIMENTAL]
    this->ctxParams.type_v = GGML_TYPE_F16; // data type for V cache [EXPERIMENTAL]
    this->ctxParams.embeddings = false;  // if true, extract embeddings (together with logits)
    this->ctxParams.offload_kqv = false; // whether to offload the KQV ops (including the KV cache) to GPU
    this->ctxParams.flash_attn = ctxParams.flash_attn;  // whether to use flash attention [EXPERIMENTAL]
    this->ctxParams.abort_callback = nullptr;
    this->ctxParams.abort_callback_data = nullptr;

    // ctx = llama_init_from_model(model, ctx_params); // TODO: this only needed if i don't reset context
    //
    // if (ctx == nullptr) {
    //     throw std::runtime_error("unable to create context");
    // }

    vocab = llama_model_get_vocab(model.get());

    sampler = nullptr;
    ctx = nullptr;

    this->name = QString::fromStdString(model->name);
}

LlamaInterface::~LlamaInterface()
{
    llama_backend_free();
}

QVector<llama_token> LlamaInterface::tokenize(const QString &prompt)
{
    int tokenCount;
    char promptCStr[8192];{};

    llama_token tokensCArr[8192]{};
    QVector<llama_token> tokens;

    strncpy(promptCStr, prompt.toLocal8Bit().data(), sizeof(promptCStr)); // TODO: figure out how to feed wide chars to llama_tokenize

    tokenCount = llama_tokenize(vocab, promptCStr, strlen(promptCStr), tokensCArr, strlen(promptCStr), true, true);

    for (int i = 0; i < tokenCount; i++) {
        tokens.append(tokensCArr[i]);
    }

    char test[8192];
    llama_detokenize(vocab, tokens.data(), tokens.size(), test, 8192, false, true);

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
        tokensPerBatch = qMin(static_cast<int>(llama_n_ubatch(ctx.get())), tokens.size() - i);
        batch = llama_batch_get_one(&tokens[i], tokensPerBatch);;

        if (llama_decode(ctx.get(), batch) != 0) {
            throw std::runtime_error("first decode failed");
        }

        i += tokensPerBatch;
    }
    end = high_resolution_clock::now();

    timeMs = duration_cast<milliseconds>(end - start).count();

    qDebug("prompt: %lld tokens", tokens.size());
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

            if (llama_decode(ctx.get(), batch)) {
                throw std::runtime_error("decode failed");
            }
        }

        newTokenId = llama_sampler_sample(sampler.get(), ctx.get(), -1);

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

    // int n_ctx; // TODO: why was it needed in the first place?
    // int n_kv_req;

    generating = true;

    resetContext(); // TODO: bad for performance, do caching instead

    tokens = tokenize(prompt);

    // n_ctx = llama_n_ctx(ctx);
    // n_kv_req = N_CTX; // TODO: smarter kv cache allocation... and also caching
    //
    // if (n_kv_req > n_ctx) {
    //     throw std::runtime_error("kv cache size is not big enough");
    // }

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

bool LlamaInterface::isGenerating() const
{
    return generating;
}

void LlamaInterface::setSamplers(SamplerArray samplers)
{
    sampler = llama_sampler_ptr(llama_sampler_chain_init({true}));

    samplers[Sampler::SAMPLER_TEMP];

    if (sampler == nullptr) {
        throw std::runtime_error("unable to init sampler chain");
    }

    qDebug(
        "Set samplers: temp: %f, top p: %f, min p: %f, top k: %u",
        samplers[Sampler::SAMPLER_TEMP].getValue().toFloat(),
        samplers[Sampler::SAMPLER_TOP_P].getValue().toFloat(),
        samplers[Sampler::SAMPLER_MIN_P].getValue().toFloat(),
        samplers[Sampler::SAMPLER_TOP_K].getValue().toInt());

    llama_sampler_chain_add(sampler.get(), llama_sampler_init_temp(samplers[Sampler::SAMPLER_TEMP].getValue().toFloat()));
    llama_sampler_chain_add(sampler.get(), llama_sampler_init_top_p(samplers[Sampler::SAMPLER_TOP_P].getValue().toFloat(), 1));
    llama_sampler_chain_add(sampler.get(), llama_sampler_init_min_p(samplers[Sampler::SAMPLER_MIN_P].getValue().toFloat(), 1));
    llama_sampler_chain_add(sampler.get(), llama_sampler_init_top_k(samplers[Sampler::SAMPLER_TOP_K].getValue().toInt()));

    llama_sampler_chain_add(sampler.get(), llama_sampler_init_dist(LLAMA_DEFAULT_SEED)); // TODO: allow setting seed
}

void LlamaInterface::resetContext()
{
    ctx = llama_context_ptr(llama_init_from_model(model.get(), ctxParams));

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
    const char *templateCStr = llama_model_chat_template(model.get(), nullptr);

    return {templateCStr};
}

QString LlamaInterface::getEOT()
{
    char buffer[1024]; // TODO: magic number
    llama_token eot = llama_vocab_eot(&model->vocab);

    llama_token_to_piece(&model->vocab, eot, buffer, sizeof(buffer), 0, true);

    return {buffer};
}
