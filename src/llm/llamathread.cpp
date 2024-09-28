#include "llamathread.h"
#include "llamainterface.h"

void llamaThread::run(std::string &modelPath, bool &running, Chat *&chatPtr)
{
    LlamaInterface llama(modelPath);

    while (running == true)
    {
        if (chatPtr)
        {
            llama.reply(*chatPtr);
            chatPtr = nullptr;
        }
    }
}

llamaThread::llamaThread(std::string modelPath)
{
    this->running = true;
    this->chatPtr = nullptr;
    this->mainThread = new std::jthread(llamaThread::run, std::ref(modelPath), std::ref(this->chatPtr), std::ref(this->chatPtr));
}

llamaThread::~llamaThread()
{
    this->running = false;
    delete this->mainThread;
}

void llamaThread::startReply(Chat &chat)
{
    this->chatPtr = &chat;
}
