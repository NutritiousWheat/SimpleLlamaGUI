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

    connect(this, &MainWindow::sendMessage, &chat, &Chat::on_messageReceived);
    connect(this, &MainWindow::interruptGeneration, &chat, &Chat::on_interruptReceived);

    connect(&chat, &Chat::appendText, this, &MainWindow::on_appendText);
    connect(&chat, &Chat::updateGUI, this, &MainWindow::on_updateGUI);
    connect(&chat, &Chat::exceptionOccured, this, &MainWindow::on_exceptionOccured);

    initSliderValues();
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
    this->ui->statusbar->showMessage("Model unloaded");
}

void MainWindow::procStateLoading()
{
    ui->submitButton->setDisabled(true);
    this->ui->statusbar->showMessage("Model loading");
}

void MainWindow::procStateIdle()
{
    ui->submitButton->setDisabled(false);
    this->ui->statusbar->showMessage("Loaded model: " + this->chat.getModelName());
}

void MainWindow::procStateGenerating()
{
    ui->submitButton->setDisabled(true); // replace with stop button
    this->ui->statusbar->showMessage("Loaded model: " + this->chat.getModelName());
}

void MainWindow::startGenerating()
{
    const QString message = ui->promptBox->toPlainText();
    const SamplersArrayT samplers = fetchSamplers();

    emit sendMessage(message, samplers);
}

void MainWindow::stopGenerating()
{
    emit interruptGeneration();
}

void MainWindow::on_submitButton_clicked()
{
    startGenerating();
    // if (llamaThread->isGenerating()) {
    //     generating = false;
    //     stopGenerating();
    //     this->ui->submitButton->setText("submit");
    // } else {
    //     generating = true;
    //     startGenerating();
    //     this->ui->submitButton->setText("stop");
    // }
}

void MainWindow::on_topKSlider_valueChanged(int value)
{
    QString stringValue = QString::number(value);

    this->ui->topKValueLabel->setText(stringValue);
}

void MainWindow::on_tempSlider_valueChanged(int value)
{
    float floatValue = value / decimalMultiplier;
    QString stringValue = QString::number(floatValue, 'f', 2);

    this->ui->tempValueLabel->setText(stringValue);
}

void MainWindow::on_topPSlider_valueChanged(int value)
{
    float floatValue = value / decimalMultiplier;
    QString stringValue = QString::number(floatValue, 'f', 2);

    this->ui->topPValueLabel->setText(stringValue);
}

void MainWindow::on_minPSlider_valueChanged(int value)
{
    float floatValue = value / decimalMultiplier;
    QString stringValue = QString::number(floatValue, 'f', 2);

    this->ui->minPValueLabel->setText(stringValue);
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
#warning just disable the button
    this->chat.clear();
}

void MainWindow::on_contextSlider_valueChanged(int value)
{
#warning not implemented
    QString stringValue = QString::number(value);

    this->ui->contextValueLabel->setText(stringValue);
}

void MainWindow::on_responseSlider_valueChanged(int value)
{
#warning not implemented
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

void MainWindow::initSliderValues() const
{
    this->ui->contextSlider->valueChanged(this->ui->contextSlider->value());
    this->ui->responseSlider->valueChanged(this->ui->responseSlider->value());

    for (int i = 0; i < SAMPLER_COUNT; i++) {
#warning make this prettier
        switch (i) {
        case TOP_K:
            this->ui->topKSlider->valueChanged(this->ui->topKSlider->value());
            break;
        case TEMP:
            this->ui->tempSlider->valueChanged(this->ui->tempSlider->value());
            break;
        case TOP_P:
            this->ui->topPSlider->valueChanged(this->ui->topPSlider->value());
            break;
        case MIN_P:
            this->ui->minPSlider->valueChanged(this->ui->minPSlider->value());
            break;
        default:
            fprintf(stderr, "you forgot about the %d sampler", i);
            break;
        }
    }
}

SamplersArrayT MainWindow::fetchSamplers() const
{
    SamplersArrayT samplers;
    for (int i = 0; i < SAMPLER_COUNT; i++) {
#warning make this prettier
        switch (i) {
        case TOP_K:
            samplers.array[i].type = TOP_K;
            samplers.array[i].value.intValue = this->ui->topKSlider->value();
            break;
        case TEMP:
            samplers.array[i].type = TEMP;
            samplers.array[i].value.floatValue = this->ui->tempSlider->value() / decimalMultiplier;
            break;
        case TOP_P:
            samplers.array[i].type = TOP_P;
            samplers.array[i].value.floatValue = this->ui->topPSlider->value() / decimalMultiplier;
            break;
        case MIN_P:
            samplers.array[i].type = MIN_P;
            samplers.array[i].value.floatValue = this->ui->minPSlider->value() / decimalMultiplier;
            break;
        default:
            fprintf(stderr, "you forgot about the %d sampler", i);
            break;
        }
    }
    return samplers;
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
