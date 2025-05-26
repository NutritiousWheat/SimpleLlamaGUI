#include <chrono>

#include "LlamaInterface.h"
#include "LlamaThread.h"

#include <iostream>

void LlamaThread::run()
{
    try {
        emit modelLoading();

        this->llama = new LlamaInterface(this->modelPath, modelParams, ctxParams);
        this->name = this->llama->getName();
        this->loaded = true;

        connect(this->llama, &LlamaInterface::tokenGenerated, this, &LlamaThread::on_tokenGenerated);
        connect(this->llama, &LlamaInterface::generationEnd, this, &LlamaThread::on_generationEnd);

        emit modelLoaded();

        while (this->loaded) {
            generationMutex.lock();
            generationCondition.wait(&generationMutex);

            if (this->loaded)
                llama->startGenerating(prompt, samplers);

            generationMutex.unlock();
        }
    } catch (std::exception &e) {
        emit exceptionOccured(e.what());
    }

    emit modelUnloaded();
    if (this->llama) {
        delete this->llama;
        this->llama = nullptr;
    }

}

LlamaThread::LlamaThread(const QString &modelPath, llama_model_params modelParams, llama_context_params ctxParams)
{
    this->llama = nullptr;
    this->loaded = false;
    this->modelPath = modelPath;
    this->modelParams = modelParams;
    this->ctxParams = ctxParams;
}

LlamaThread::~LlamaThread()
{
    this->loaded = false;
    this->generationCondition.wakeOne();
    this->wait();
}

void LlamaThread::startGenerating(const QString &prompt, const SamplerArray &samplers)
{
    this->prompt = prompt;
    this->samplers = samplers;
    generationCondition.wakeOne();
}

void LlamaThread::interruptGeneration()
{
    llama->interruptGeneration();
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

QString LlamaThread::getEOT()
{
    if (this->llama)
        return llama->getEOT();

    return "";
}
