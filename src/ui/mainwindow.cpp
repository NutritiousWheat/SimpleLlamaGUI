#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <qobject.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
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

void MainWindow::on_topKSlider_valueChanged(int value)
{
    QString stringValue = QString::number(value);

    this->ui->topKValueLabel->setText(stringValue);
    if (this->llamaThread) this->llamaThread->updateSampler({static_cast<float>(value), TOP_K});
#warning maybe no static casts?
}

void MainWindow::on_tempSlider_valueChanged(int value)
{
    float floatValue = value / decimalMultiplier;
    QString stringValue = QString::number(floatValue, 'f', 2);

    this->ui->tempValueLabel->setText(stringValue);
    if (this->llamaThread) this->llamaThread->updateSampler({floatValue, TEMP});
}


void MainWindow::on_topPSlider_valueChanged(int value)
{
    float floatValue = value / decimalMultiplier;
    QString stringValue = QString::number(floatValue, 'f', 2);

    this->ui->topPValueLabel->setText(stringValue);
    if (this->llamaThread) this->llamaThread->updateSampler({floatValue, TOP_P});
}


void MainWindow::on_minPSlider_valueChanged(int value)
{
    float floatValue = value / decimalMultiplier;
    QString stringValue = QString::number(floatValue, 'f', 2);

    this->ui->minPValueLabel->setText(stringValue);
    if (this->llamaThread) this->llamaThread->updateSampler({floatValue, MIN_P});
}

#warning THIS IS FUCKING GARBAGE!!!!!
void MainWindow::refreshChat(void)
{
    emit refresh();
}

void MainWindow::on_refresh()
{
    if (this->llamaThread->isGenerating())
    {
        this->ui->submitButton->setText("stop");
    }
    else
    {
        this->ui->submitButton->setText("submit");
    }
    this->ui->chatBox->setText(QString::fromStdString(chat.getString()));
}

#warning mind the edge cases
void MainWindow::on_loadButton_clicked()
{    QString path = this->ui->pathLine->text();
    std::function<void(void)> refreshFunc = std::bind(&MainWindow::refreshChat, this);

    QObject::connect(this, &MainWindow::refresh,
                     this, &MainWindow::on_refresh);

    if (llamaThread) delete llamaThread;
    llamaThread = new LlamaThread(path.toStdString(), refreshFunc);

    for (int i = 0; i < SAMPLER_COUNT; i++)
    {
#warning make this prettier
        switch (i)
        {
        case TOP_K:
            this->ui->topKSlider->valueChanged(this->ui->tempSlider->value());
            break;
        case TEMP:
            this->ui->tempSlider->valueChanged(this->ui->tempSlider->value());
            break;
        case TOP_P:
            this->ui->topPSlider->valueChanged(this->ui->tempSlider->value());
            break;
        case MIN_P:
            this->ui->minPSlider->valueChanged(this->ui->tempSlider->value());
            break;
        default:
            fprintf(stderr, "you forgot about the %d sampler", i);
            break;
        }
    }
}


void MainWindow::on_clearButton_pressed()
{
#warning just disable the button
    if (this->llamaThread && this->llamaThread->isGenerating()) return;
    this->chat.clear();
    emit refresh();
}

