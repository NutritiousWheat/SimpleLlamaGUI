#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "../chat/chat.h"
#include "../llm/llamainterface.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Chat chat;
    LlamaInterface *llama;
    bool generating = false;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_submitButton_clicked();

private:
    Ui::MainWindow *ui;

    void startGenerating();
    void stopGenerating();
};
#endif // MAINWINDOW_H
