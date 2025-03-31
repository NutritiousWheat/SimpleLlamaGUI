#ifndef LLAMATHREAD_H
#define LLAMATHREAD_H

#include <thread>

#include "../chat/Chat.h"
#include "LlamaInterface.h"

class LlamaThread
{
private:
    LlamaInterface *llama;
    std::string modelPath;
    std::function<void(void)> refreshChat;

    std::jthread *mainThread;
    bool running;

    Chat *chatPtr;

    Sampler samplers[SAMPLER_COUNT]{};
    bool samplersChanged;

    std::string name;

    static void run(LlamaThread *llamaThread);

public:
    LlamaThread(const std::string &modelPath, const std::function<void(void)> &refreshChat);
    ~LlamaThread();

    void startReply(Chat &chat);
    void stopReply();
    bool isGenerating();
    void updateSampler(Sampler sampler);
    std::string getName();
};

#endif // LLAMATHREAD_H
