#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "../chat/chat.h"
#include "../llm/llamathread.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
  public:
    void updateChat(std::string text);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void on_submitButton_clicked();

      void on_refreshButton_clicked();

  private:
    Ui::MainWindow *ui;

    Chat chat;
    LlamaThread *llamaThread;
    bool generating = false;

    void startGenerating();
    void stopGenerating();
};
#endif // MAINWINDOW_H
