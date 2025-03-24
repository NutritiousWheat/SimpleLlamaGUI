#include "llamainterface.h"
#include <stdexcept>

#define N_PREDICT 250
#define N_CTX 8192
#define N_BATCH 2048
#define N_UBATCH 512

#define THREADS 16

LlamaInterface::LlamaInterface(std::string modelPath, std::function<void(void)> refreshChat)
{
    this->refreshChat = refreshChat;

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
    params.use_mmap = true;
    params.use_mlock = false;
    params.check_tensors = false;

    model = llama_model_load_from_file(modelPath.c_str(), params);

    if (model == nullptr) {
        throw std::runtime_error("unable to load model");
    }

    ctx_params.n_ctx = N_CTX;     // text context, 0 = from model
    ctx_params.n_batch = N_BATCH; // logical maximum batch size that can be submitted to llama_decode
    ctx_params.n_ubatch = N_UBATCH; // physical maximum batch size
    ctx_params.n_seq_max = 1; // max number of sequences (i.e. distinct states for recurrent models)
    ctx_params.n_threads = THREADS;       // number of threads to use for generation
    ctx_params.n_threads_batch = THREADS; // number of threads to use for batch processing
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
    ctx_params.logits_all
        = false; // the llama_decode() call computes all logits, not just the last one (DEPRECATED - set llama_batch.logits instead)
    ctx_params.embeddings = false;  // if true, extract embeddings (together with logits)
    ctx_params.offload_kqv = false; // whether to offload the KQV ops (including the KV cache) to GPU
    ctx_params.flash_attn = false;  // whether to use flash attention [EXPERIMENTAL]
    ctx_params.abort_callback = nullptr;
    ctx_params.abort_callback_data = nullptr;
#warning context doesnt get cleared
    ctx = llama_init_from_model(model, ctx_params);

    if (ctx == nullptr) {
        throw std::runtime_error("unable to create context");
    }

    sampler = llama_sampler_chain_init({true}); // no_perf = true

    if (sampler == nullptr) {
        throw std::runtime_error("unable to init sampler chain");
    }

    llama_sampler_chain_add(sampler, llama_sampler_init_greedy());

    batch = llama_batch_init(N_UBATCH, 0, 1);

#warning get actual name from metadata
    this->name = modelPath;
}

LlamaInterface::~LlamaInterface()
{
    llama_batch_free(batch);
    llama_sampler_free(sampler);
    llama_free(ctx);
    llama_model_free(model);
    llama_backend_free();
}

std::string LlamaInterface::promptify(Chat &chat)
{
    llama_chat_message *llama_chat;
    std::string prompt;
    char buffer[N_CTX];
    const char *tmpl = llama_model_chat_template(model, nullptr);

    chat.getAllMessages(&llama_chat);

    llama_chat_apply_template(tmpl, llama_chat, chat.size(), false, buffer, N_CTX);

    prompt = std::string(buffer);

    delete[] llama_chat;

    fprintf(stderr, "Prompt: %s\n", prompt.c_str());

    return prompt;
}

void LlamaInterface::reply(Chat &chat)
{
    std::string prompt;
    llama_token *tokens;
    const llama_vocab *vocab;

    int n_tokens;
    int n_ctx;
    int n_kv_req;

    int n_cur;
    int n_chars;
    int n_decode;

    llama_token new_token_id;
    char buffer[16] = {0};

    prompt = promptify(chat);

    tokens = new llama_token[prompt.size()];

    vocab = llama_model_get_vocab(model);

    n_tokens
        = llama_tokenize(vocab, prompt.c_str(), prompt.size(), tokens, prompt.size(), true, true);
    n_ctx = llama_n_ctx(ctx);
    n_kv_req = N_CTX + (N_PREDICT - N_CTX);

    if (n_kv_req > n_ctx) {
        throw std::runtime_error("kv cache size is not big enough");
    }

    for (size_t i = 0; i < n_tokens; i++) {
        batch.token[batch.n_tokens] = tokens[i];
        batch.pos[batch.n_tokens] = i;

        batch.n_seq_id[batch.n_tokens] = 1;
        batch.seq_id[batch.n_tokens][0] = 0;

        batch.logits[batch.n_tokens] = false;

        batch.n_tokens++;
    }

    batch.logits[batch.n_tokens - 1] = true;

    delete[] tokens;

    if (llama_decode(ctx, batch) != 0) {
        throw std::runtime_error("first decode failed");
    }

    n_cur = batch.n_tokens;
    n_decode = 0;

    chat.appendLLMMessage("");

    while (n_cur <= N_PREDICT) {
        // sample the next token
        {
            const llama_token new_token_id = llama_sampler_sample(sampler, ctx, batch.n_tokens - 1);

            // is it the end?
            if (llama_vocab_is_eog(vocab, new_token_id) || n_cur == N_PREDICT || this->forceStop) {
                chat.continueMessage("\n");
                this->forceStop = false;
                break;
            }

            n_chars = llama_token_to_piece(vocab, new_token_id, buffer, sizeof(buffer), 0, false);
            buffer[n_chars] = '\0'; // llama_token_to_piece does not null-terminate
            chat.continueMessage(buffer);

            // prepare the next batch
            batch.n_tokens = 0;

            // push this new token for next evaluation
            batch.token[batch.n_tokens] = new_token_id;
            batch.pos[batch.n_tokens] = n_cur;
            batch.n_seq_id[batch.n_tokens] = 1;
            batch.seq_id[batch.n_tokens][0] = 0;
            batch.logits[batch.n_tokens] = true;
            batch.n_tokens++;

            n_decode += 1;
        }

        n_cur += 1;

        // evaluate the current batch with the transformer model
        if (llama_decode(ctx, batch)) {
            throw std::runtime_error("decode failed");
        }
        this->refreshChat();
    }
}

void LlamaInterface::stop()
{
    this->forceStop = true;
}

void LlamaInterface::updateSamplers(Sampler samplers[SAMPLER_COUNT])
{
    if (sampler)
        free(sampler);

    sampler = llama_sampler_chain_init({true}); // no_perf = true

    if (sampler == nullptr) {
        throw std::runtime_error("unable to init sampler chain");
    }

    fprintf(
        stderr,
        "temp: %f, top p: %f, min p: %f, top k: %u\n",
        samplers[TEMP].value.floatValue,
        samplers[TOP_P].value.floatValue,
        samplers[MIN_P].value.floatValue,
        samplers[TOP_K].value.intValue);

    llama_sampler_chain_add(sampler, llama_sampler_init_temp(samplers[TEMP].value.floatValue));
    llama_sampler_chain_add(sampler, llama_sampler_init_top_p(samplers[TOP_P].value.floatValue, 1));
    llama_sampler_chain_add(sampler, llama_sampler_init_min_p(samplers[MIN_P].value.floatValue, 1));
    llama_sampler_chain_add(sampler, llama_sampler_init_top_k(samplers[TOP_K].value.intValue));

    llama_sampler_chain_add(sampler, llama_sampler_init_dist(rng.getRand()));
}

std::string LlamaInterface::getName()
{
    return this->name;
}
