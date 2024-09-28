#ifndef LLAMATHREAD_H
#define LLAMATHREAD_H

#include <thread>

#include "../chat/chat.h"

class llamaThread
{
    std::jthread *mainThread;
    bool running;
    Chat *chatPtr;

    static void run(std::string &modelPath, bool &running, Chat *&chatPtr);

public:
    llamaThread(std::string modelPath);
    ~llamaThread();

    void startReply(Chat &chat);
};

#endif // LLAMATHREAD_H
