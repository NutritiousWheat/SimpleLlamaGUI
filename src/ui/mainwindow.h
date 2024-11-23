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

    void on_tempSlider_valueChanged(int value);

    void on_topKSlider_valueChanged(int value);

    void on_topPSlider_valueChanged(int value);

    void on_minPSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    Chat chat;
    LlamaThread *llamaThread;
    bool generating = false;

    float decimalMultiplier = 100.f;

    void startGenerating();
    void stopGenerating();
};
#endif // MAINWINDOW_H
