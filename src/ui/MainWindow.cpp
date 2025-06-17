#include "MainWindow.h"

#include <QDir> // TODO: include code style
#include <QMessageBox>
#include <cmath>

#include "./ui_MainWindow.h"
#include <QObject>

#include "BoolParamWidget.h"
#include "MessageWidget.h"
#include "NumParamWidget.h"
#include "SamplerWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int samplersColumns = sqrt(Sampler::SamplerTypeE::SAMPLERS_COUNT);

    for (int i = 0; i < Sampler::SamplerTypeE::SAMPLERS_COUNT; i++) {
        auto type = static_cast<Sampler::SamplerTypeE>(i);

        auto samplerLayout = dynamic_cast<QGridLayout *>(ui->tabParams->layout());
        auto samplerWidget = new SamplerWidget(ui->tabParams, type);

        samplerLayout->addWidget(samplerWidget, i % samplersColumns, i / samplersColumns);

        samplers.insert(type, samplerWidget);
    }

    ui->boolAreaWidgetContents->layout()->setAlignment(Qt::AlignTop);

    for (int i = 0; i < BoolParamWidget::BoolParamE::BOOL_PARAM_COUNT; i++) {
        auto type = static_cast<BoolParamWidget::BoolParamE>(i);

        auto boolParamWidget = new BoolParamWidget(ui->tabParams, type); // TODO: smart pointers
        ui->boolAreaWidgetContents->layout()->addWidget(boolParamWidget);

        boolParams.insert(type, boolParamWidget);
    }

    ui->numAreaWidgetContents->layout()->setAlignment(Qt::AlignTop);

    for (int i = 0; i < NumParamWidget::NumParamE::NUM_PARAM_COUNT; i++) {
        auto type = static_cast<NumParamWidget::NumParamE>(i);

        auto numParamWidget = new NumParamWidget(ui->tabParams, type);
        ui->numAreaWidgetContents->layout()->addWidget(numParamWidget);

        numParams.insert(type, numParamWidget);
    }

    connect(&chat, &Chat::appendText, this, &MainWindow::on_appendText);
    connect(&chat, &Chat::newMessage, this, &MainWindow::on_appendMessage);
    connect(&chat, &Chat::updateGUI, this, &MainWindow::on_updateGUI);
    connect(&chat, &Chat::exceptionOccured, this, &MainWindow::on_exceptionOccured);

    procStateNotLoaded();
    refreshModels();

    chatLayout.setAlignment(Qt::AlignTop);
    ui->chatAreaWidgetContents->setLayout(&chatLayout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::procStateNotLoaded()
{
    ui->submitButton->setDisabled(true);
    ui->submitButton->setText("Submit");

    ui->continueButton->setDisabled(true);

    ui->clearButton->setDisabled(false);

    ui->loadButton->setDisabled(false);
    ui->loadButton->setText("Load");

    ui->statusbar->showMessage("Model unloaded");
}

void MainWindow::procStateLoading()
{
    ui->submitButton->setDisabled(true);
    ui->submitButton->setText("Submit");

    ui->continueButton->setDisabled(true);

    ui->clearButton->setDisabled(false);

    ui->loadButton->setDisabled(true);
    ui->loadButton->setText("Unload");

    ui->statusbar->showMessage("Model loading");
}

void MainWindow::procStateIdle()
{
    ui->submitButton->setDisabled(false);
    ui->submitButton->setText("Submit");

    ui->continueButton->setDisabled(false);

    ui->clearButton->setDisabled(false);

    ui->loadButton->setDisabled(false);
    ui->loadButton->setText("Unload");

    ui->statusbar->showMessage("Loaded model: " + chat.getModelName());
}

void MainWindow::procStateGenerating()
{
    ui->submitButton->setDisabled(false);
    ui->submitButton->setText("Stop");

    ui->continueButton->setDisabled(true);

    ui->clearButton->setDisabled(true);

    ui->loadButton->setDisabled(true);
    ui->loadButton->setText("Unload");

    ui->statusbar->showMessage("Loaded model (generating): " + chat.getModelName());
}

void MainWindow::startGenerating()
{
    const QString message = ui->promptBox->toPlainText();
    const SamplerArray samplerArray = getSamplerArray();

    chat.reply(message, samplerArray);

    ui->promptBox->setText("");
}

void MainWindow::stopGenerating()
{
    chat.interruptGeneration();
}

void MainWindow::on_submitButton_clicked()
{ // TODO: do not use QString comparison here... just store the current state in some variable in MainWindow
    if (ui->submitButton->text() == "Stop") {
        stopGenerating();
    } else {
        startGenerating();
    }
}

void MainWindow::on_continueButton_clicked()
{
    const SamplerArray samplerArray = getSamplerArray();

    this->chat.continueLastMessage(samplerArray);
}


void MainWindow::on_appendText(QString &text)
{
    this->messages.last()->appendText(text);
}

void MainWindow::on_appendMessage(const QString &text, Chat::messageRoleE role)
{
    this->messages.append(new MessageWidget(this->ui->chatArea, role));
    this->chatLayout.addWidget(this->messages.last());
    this->messages.last()->appendText(text);
}

void MainWindow::on_updateGUI(Chat::chatStateE state)
{
    switch (state) {
    case Chat::CHAT_STATE_NOT_LOADED:
        procStateNotLoaded();
        break;
    case Chat::CHAT_STATE_LOADING:
        procStateLoading();
        break;
    case Chat::CHAT_STATE_IDLE:
        procStateIdle();
        break;
    case Chat::CHAT_STATE_GENERATING:
        procStateGenerating();
        break;
    default:
        throw std::runtime_error("Unknown state");
    }
}

void MainWindow::on_exceptionOccured(const QString &errorMsg)
{
    QMessageBox::warning(nullptr, ERR_HEADER, "Exception occured:\n" + errorMsg);
}

void MainWindow::on_loadButton_clicked()
{
    // TODO: do not use QString comparison here... just store the current state in some variable in MainWindow
    if (ui->loadButton->text() == "Load") {
        QString name = ui->modelBox->itemText(ui->modelBox->currentIndex());
        QString path = config.getValue(ConfigApp::ModelDir) + "/" + name;

        chat.loadModel(path, getModelParams(), getCtxParams());
        config.setValue(ConfigApp::LastUsedModel, name);
    } else {
        chat.unloadModel();
    }
}

void MainWindow::on_clearButton_pressed()
{
    this->chat.clear();
    for (const auto message : messages) {
        this->chatLayout.removeWidget(message);
        delete message;
    }
    messages.clear();

}

void MainWindow::on_refreshButton_clicked()
{
    refreshModels();
}

void MainWindow::refreshModels()
{
    QString lastUsedModel = config.getValue(ConfigApp::LastUsedModel);
    QString modelDirPath = config.getValue(ConfigApp::ModelDir);
    QDir modelDir(modelDirPath);
    QList fileList = modelDir.entryList(QDir::Files, QDir::Name);

    ui->modelBox->clear();
    for (auto &filePath : fileList) {
        ui->modelBox->addItem(filePath);
    }

    if (fileList.contains(lastUsedModel)) {
        ui->modelBox->setCurrentText(lastUsedModel);
    }
}

SamplerArray MainWindow::getSamplerArray()
{
    SamplerArray samplerArray;
    Sampler::SamplerTypeE type;

    for (int i = 0; i < samplers.count(); i++) {
        type = static_cast<Sampler::SamplerTypeE>(i);
        samplerArray[type] = samplers[type]->getValue();
    }

    return samplerArray;
}

llama_model_params MainWindow::getModelParams()
{
    llama_model_params params{};

    params.n_gpu_layers = numParams[NumParamWidget::NumParamE::N_GPU_LAYERS]->getValue();

    return params;
}

llama_context_params MainWindow::getCtxParams()
{
    llama_context_params params{};

    params.n_ctx = numParams[NumParamWidget::NumParamE::N_CONTEXT_TOKENS]->getValue();
    params.n_threads = numParams[NumParamWidget::NumParamE::N_THREADS]->getValue();
    params.n_threads_batch = numParams[NumParamWidget::NumParamE::N_THREADS]->getValue(); // TODO: separate threads and batch threads
    params.flash_attn = boolParams[BoolParamWidget::BoolParamE::USE_FLASH_ATTENTION]->getValue();

    return params;
}
