#include "MainWindow.h"

#include <QDir> // TODO: include code style
#include <QMessageBox>

#include "./ui_MainWindow.h"
#include <QObject>

#include "MessageWidget.h"
#include "SamplerWidget.h"
#include "BoolParamWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int samplersColumns = sqrt(Sampler::SamplerTypeE::SAMPLERS_COUNT);

    for (int i = 0; i < Sampler::SamplerTypeE::SAMPLERS_COUNT; i++) {
        QGridLayout *samplerLayout = static_cast<QGridLayout *>(ui->tabParams->layout());
        SamplerWidget *samplerWidget = new SamplerWidget(ui->tabParams, static_cast<Sampler::SamplerTypeE>(i));

        samplerLayout->addWidget(samplerWidget, i % samplersColumns, i / samplersColumns);

        samplers.append(samplerWidget);
    }

    ui->boolAreaWidgetContents->layout()->setAlignment(Qt::AlignTop);

    for (int i = 0; i < BoolParamWidget::BoolParamE::BOOL_PARAM_COUNT; i++) {
        BoolParamWidget *boolParamWidget = new BoolParamWidget(ui->tabParams, static_cast<BoolParamWidget::BoolParamE>(i));
        ui->boolAreaWidgetContents->layout()->addWidget(boolParamWidget);
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

    ui->statusbar->showMessage("Model unloaded");
}

void MainWindow::procStateLoading()
{
    ui->submitButton->setDisabled(true);
    ui->submitButton->setText("Submit");

    ui->continueButton->setDisabled(true);

    ui->clearButton->setDisabled(false);

    ui->statusbar->showMessage("Model loading");
}

void MainWindow::procStateIdle()
{
    ui->submitButton->setDisabled(false);
    ui->submitButton->setText("Submit");

    ui->continueButton->setDisabled(false);

    ui->clearButton->setDisabled(false);

    ui->statusbar->showMessage("Loaded model: " + chat.getModelName());
}

void MainWindow::procStateGenerating()
{
    ui->submitButton->setDisabled(false);
    ui->submitButton->setText("Stop");

    ui->continueButton->setDisabled(true);

    ui->clearButton->setDisabled(true);

    ui->statusbar->showMessage("Loaded model (generating): " + chat.getModelName());
}

void MainWindow::startGenerating()
{
    const QString message = ui->promptBox->toPlainText();
    const SamplerArray samplerArray = getSamplerArray();

    chat.reply(message, samplerArray);
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

void MainWindow::on_exceptionOccured(QString errorMsg)
{
    QMessageBox::warning(nullptr, ERR_HEADER, "Exception occured:\n" + errorMsg);
}

void MainWindow::on_loadButton_clicked()
{
    QString name = ui->modelBox->itemText(ui->modelBox->currentIndex());
    QString path = config.getValue(ConfigApp::ModelDir) + "/" + name;

    chat.loadModel(path);

    config.setValue(ConfigApp::LastUsedModel, name);
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

void MainWindow::on_unloadButton_clicked()
{
    chat.unloadModel();
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
    for (int i = 0; i < fileList.size(); i++) {
        ui->modelBox->addItem(fileList[i]);
    }

    if (fileList.contains(lastUsedModel)) {
        ui->modelBox->setCurrentText(lastUsedModel);
    }
}

SamplerArray MainWindow::getSamplerArray()
{
    SamplerArray samplerArray;

    for (int i = 0; i < samplers.count(); i++) {
        samplerArray[static_cast<Sampler::SamplerTypeE>(i)] = samplers[i]->getValue();
    }

    return samplerArray;
}
