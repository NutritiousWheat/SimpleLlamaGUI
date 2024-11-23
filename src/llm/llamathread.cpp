#include "llamathread.h"
#include "llamainterface.h"

void LlamaThread::run(LlamaThread *llamaThread)
{
    llamaThread->llama = new LlamaInterface(llamaThread->modelPath);

    while (llamaThread->running == true)
    {
        if (llamaThread->samplersChanged)
        {
#warning NOT THREAD SAFE!!!!
            llamaThread->llama->updateSamplers(llamaThread->samplers);
            llamaThread->samplersChanged = false;
        }
        if (llamaThread->chatPtr)
        {
            llamaThread->llama->reply(*llamaThread->chatPtr);
            llamaThread->chatPtr = nullptr;
        }
    }

    delete llamaThread->llama;
}

LlamaThread::LlamaThread(std::string modelPath)
{
    this->running = true;
    this->chatPtr = nullptr;
    this->modelPath = modelPath;
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
#warning add change delta time to prevent recreating sampler chain on every minor change
    this->samplers[sampler.type].value = sampler.value;
    this->samplersChanged = true;
}
