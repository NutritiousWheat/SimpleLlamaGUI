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
}

void MainWindow::on_submitButton_clicked()
{
    if (llamaThread->isGenerating())
    {
        generating = false;
        stopGenerating();
    }
    else
    {
        generating = true;
        startGenerating();
    }
}

void MainWindow::on_refreshButton_clicked()
{
    ui->chatBox->setText(QString::fromStdString(chat.getString()));
}

