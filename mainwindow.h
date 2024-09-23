#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "chat.h"
#include "llamainterface.h"

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

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void generate();

private slots:
    void on_generateButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
