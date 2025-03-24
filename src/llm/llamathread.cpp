#include <chrono>

#include "llamainterface.h"
#include "llamathread.h"

static constexpr std::chrono::duration tick = std::chrono::milliseconds(10);

void LlamaThread::run(LlamaThread *llamaThread)
{
    llamaThread->llama = new LlamaInterface(llamaThread->modelPath, llamaThread->refreshChat);
    llamaThread->name = llamaThread->llama->getName();

    while (llamaThread->running == true) {
        if (llamaThread->samplersChanged) {
#warning not thread safe
            llamaThread->llama->updateSamplers(llamaThread->samplers);
            llamaThread->samplersChanged = false;
        }
        if (llamaThread->chatPtr) {
            llamaThread->llama->reply(*llamaThread->chatPtr);
            llamaThread->chatPtr = nullptr;
            llamaThread->refreshChat();
        }
        std::this_thread::sleep_for(tick);
    }

    delete llamaThread->llama;
}

LlamaThread::LlamaThread(const std::string &modelPath, const std::function<void(void)> &refreshChat)
{
    this->llama = nullptr;
    this->samplersChanged = false;
    this->running = true;
    this->chatPtr = nullptr;
    this->modelPath = modelPath;
    this->refreshChat = refreshChat;
    this->mainThread = new std::jthread(LlamaThread::run, this);
}

LlamaThread::~LlamaThread()
{
    this->running = false;
    delete this->mainThread;
}

void LlamaThread::startReply(Chat &chat)
{
    this->chatPtr = &chat;
}

void LlamaThread::stopReply()
{
    this->llama->stop();
}

bool LlamaThread::isGenerating()
{
    return this->chatPtr != nullptr;
}

void LlamaThread::updateSampler(Sampler sampler)
{
    this->samplers[sampler.type].value = sampler.value;
    this->samplersChanged = true;
}

std::string LlamaThread::getName()
{
    return this->name;
}
