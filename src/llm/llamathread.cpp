#include "llamathread.h"
#include "llamainterface.h"

void LlamaThread::run(LlamaThread *llamaThread)
{
    LlamaInterface llama(llamaThread->modelPath);

    while (llamaThread->running == true)
    {
        if (llamaThread->chatPtr)
        {
            llama.reply(*llamaThread->chatPtr);
            llamaThread->chatPtr = nullptr;
        }
    }
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

bool LlamaThread::isGenerating()
{
    return this->chatPtr != nullptr;
}
