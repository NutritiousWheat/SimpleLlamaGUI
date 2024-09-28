#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    llama = new LlamaInterface("/home/potato/code_projects/diplom/build/Desktop-Debug/Phi-3.5-mini-instruct-Q4_K_L.gguf");
    chat.appendSystemMessage("You are a helpful assistant. Assist with whatever user requires.");
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete llama;
}

void MainWindow::startGenerating()
{
    chat.appendUserMessage(ui->promptBox->toPlainText().toStdString());
    llama->reply(chat);
    ui->chatBox->setPlainText(QString::fromStdString(chat.getString()));
}

void MainWindow::stopGenerating()
{
    chat.appendUserMessage(ui->promptBox->toPlainText().toStdString());
    llama->reply(chat);
    ui->chatBox->setPlainText(QString::fromStdString(chat.getString()));
}

void MainWindow::on_submitButton_clicked()
{
    if (generating)
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


