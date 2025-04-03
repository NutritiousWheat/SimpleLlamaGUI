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
            emit replyStart(prompt, samplers);
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

void LlamaThread::on_replyStart(QString prompt, SamplersArrayT samplers)
{
    this->prompt = prompt;
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
    if (this->llama)
        return this->llama->isGenerating();

    return false;
}

QString LlamaThread::getName()
{
    return this->name;
}

QString LlamaThread::getTemplate()
{
    if (this->llama)
        return llama->getTemplate();

    return "";
}
