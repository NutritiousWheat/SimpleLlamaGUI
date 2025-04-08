#include "MainWindow.h"

#include <QDir>
#include <QMessageBox>

#include "./ui_MainWindow.h"
#include <QObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // TODO: connect from sampler widget
    connect(ui->samplerWidget_1, &SamplerWidget::samplerUpdated, this, &MainWindow::on_sampler_valueChanged);
    connect(ui->samplerWidget_2, &SamplerWidget::samplerUpdated, this, &MainWindow::on_sampler_valueChanged);
    connect(ui->samplerWidget_3, &SamplerWidget::samplerUpdated, this, &MainWindow::on_sampler_valueChanged);
    connect(ui->samplerWidget_4, &SamplerWidget::samplerUpdated, this, &MainWindow::on_sampler_valueChanged);

    connect(&chat, &Chat::appendText, this, &MainWindow::on_appendText);
    connect(&chat, &Chat::updateGUI, this, &MainWindow::on_updateGUI);
    connect(&chat, &Chat::exceptionOccured, this, &MainWindow::on_exceptionOccured);

    procStateNotLoaded();
    refreshModels();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::procStateNotLoaded()
{
    ui->submitButton->setDisabled(true);
    ui->submitButton->setText("Submit");

    ui->clearButton->setDisabled(false);

    this->ui->statusbar->showMessage("Model unloaded");
}

void MainWindow::procStateLoading()
{
    ui->submitButton->setDisabled(true);
    ui->submitButton->setText("Submit");

    ui->clearButton->setDisabled(false);

    this->ui->statusbar->showMessage("Model loading");
}

void MainWindow::procStateIdle()
{
    ui->submitButton->setDisabled(false);
    ui->submitButton->setText("Submit");

    ui->clearButton->setDisabled(false);

    this->ui->statusbar->showMessage("Loaded model: " + this->chat.getModelName());
}

void MainWindow::procStateGenerating()
{
    ui->submitButton->setDisabled(false);
    ui->submitButton->setText("Stop");

    ui->clearButton->setDisabled(true);

    this->ui->statusbar->showMessage("Loaded model (generating): " + this->chat.getModelName());
}

void MainWindow::startGenerating()
{
    const QString message = ui->promptBox->toPlainText();

    chat.reply(message, samplers);
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

void MainWindow::on_sampler_valueChanged(const Sampler sampler)
{
    samplers[sampler.getType()] = sampler;
}

void MainWindow::on_appendText()
{
    this->ui->chatBox->setText(chat.getString());
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
    this->ui->chatBox->setText("");
}

void MainWindow::on_contextSlider_valueChanged(int value)
{
// TODO: not implemented
    QString stringValue = QString::number(value);

    this->ui->contextValueLabel->setText(stringValue);
}

void MainWindow::on_responseSlider_valueChanged(int value)
{
// TODO: not implemented
    QString stringValue = QString::number(value);

    this->ui->responseValueLabel->setText(stringValue);
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
