#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <qobject.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    chat.appendSystemMessage("You are a helpful assistant. Assist with whatever user requires.");

    llamaThread =
        new LlamaThread("/home/potato/code_projects/diplom/build/Desktop-Debug/Phi-3.5-mini-instruct-Q4_K_L.gguf");

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete llamaThread;
}

void MainWindow::startGenerating()
{
    chat.appendUserMessage(ui->promptBox->toPlainText().toStdString());
    llamaThread->startReply(chat);
}

void MainWindow::stopGenerating()
{
    llamaThread->stopReply();
}

void MainWindow::on_submitButton_clicked()
{
    if (llamaThread->isGenerating())
    {
        generating = false;
        stopGenerating();
        this->ui->submitButton->setText("submit");
    }
    else
    {
        generating = true;
        startGenerating();
        this->ui->submitButton->setText("stop");
    }
}

void MainWindow::on_refreshButton_clicked()
{
    if (!llamaThread->isGenerating())
    {
        this->ui->submitButton->setText("submit");
    }
    ui->chatBox->setText(QString::fromStdString(chat.getString()));
}


void MainWindow::on_topKSlider_valueChanged(int value)
{
    QString stringValue = QString::number(value);

    this->ui->topKValueLabel->setText(stringValue);
    this->llamaThread->updateSampler({static_cast<float>(value), TEMP});
#warning maybe no static casts?
}

void MainWindow::on_tempSlider_valueChanged(int value)
{
    float floatValue = value / decimalMultiplier;
    QString stringValue = QString::number(floatValue, 'f', 2);

    this->ui->tempValueLabel->setText(stringValue);
    this->llamaThread->updateSampler({floatValue, TEMP});
}


void MainWindow::on_topPSlider_valueChanged(int value)
{
    float floatValue = value / decimalMultiplier;
    QString stringValue = QString::number(floatValue, 'f', 2);

    this->ui->topPValueLabel->setText(stringValue);
    this->llamaThread->updateSampler({floatValue, TEMP});
}


void MainWindow::on_minPSlider_valueChanged(int value)
{
    float floatValue = value / decimalMultiplier;
    QString stringValue = QString::number(floatValue, 'f', 2);

    this->ui->minPValueLabel->setText(stringValue);
    this->llamaThread->updateSampler({floatValue, TEMP});
}

