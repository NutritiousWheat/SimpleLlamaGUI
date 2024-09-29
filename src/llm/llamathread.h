#ifndef LLAMATHREAD_H
#define LLAMATHREAD_H

#include <thread>

#include "../chat/chat.h"

class LlamaThread
{
  private:
    std::jthread *mainThread;
    std::string modelPath;
    bool running;
    Chat *chatPtr;

    static void run(LlamaThread *llamaThread);

  public:
    LlamaThread(std::string modelPath);
    ~LlamaThread();

    void startReply(Chat &chat);
    bool isGenerating();
};

#endif // LLAMATHREAD_H
