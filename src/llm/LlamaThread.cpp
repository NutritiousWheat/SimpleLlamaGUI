#include <chrono>

#include "LlamaInterface.h"
#include "LlamaThread.h"

#include <iostream>

void LlamaThread::run()
{
    try {
        this->llama = new LlamaInterface(this->modelPath);
        this->name = this->llama->getName();
        this->loaded = true;

        connect(this, &LlamaThread::replyStart, this->llama, &LlamaInterface::on_replyStart);
        connect(this, &LlamaThread::replyStop, this->llama, &LlamaInterface::on_replyStop);

        connect(this->llama, &LlamaInterface::tokenGenerated, this, &LlamaThread::on_tokenGenerated);
        connect(this->llama, &LlamaInterface::generationEnd, this, &LlamaThread::on_generationEnd);

        while (this->loaded) {
            generationMutex.lock();
            generationCondition.wait(&generationMutex);
            emit replyStart(messages, samplers);
            generationMutex.unlock();
        }

    } catch (std::exception &e) {
        std::cerr << "llama.cpp error: " << e.what() << std::endl;
#warning add error window
    }

    if (this->llama)
        delete this->llama;
}

LlamaThread::LlamaThread(const QString &modelPath)
{
    this->llama = nullptr;
    this->loaded = false;
    this->modelPath = modelPath;
}

LlamaThread::~LlamaThread()
{
    if (this->llama)
        delete this->llama;

    this->loaded = false;
}

void LlamaThread::on_replyStart(const QVector <llama_chat_message> &messages, const SamplersArrayT &samplers)
{
    this->messages = messages;
    this->samplers = samplers;
    generationCondition.wakeOne();
}

void LlamaThread::on_replyStop()
{
    emit replyStop();
}

void LlamaThread::on_tokenGenerated(QString token)
{
    emit tokenGenerated(token);
}

void LlamaThread::on_generationEnd()
{
    emit generationEnd();
}

bool LlamaThread::isGenerating()
{
    return this->llama->isGenerating();
}

QString LlamaThread::getName()
{
    return this->name;
}
