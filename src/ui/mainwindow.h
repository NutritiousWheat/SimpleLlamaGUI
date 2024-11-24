#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "../chat/chat.h"
#include "../llm/llamathread.h"

#warning TODO
#warning use QThread for llamathread
#warning use QObject for the rest of the classes
#warning do signals and slots


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

    void on_tempSlider_valueChanged(int value);

    void on_topKSlider_valueChanged(int value);

    void on_topPSlider_valueChanged(int value);

    void on_minPSlider_valueChanged(int value);

    void on_loadButton_clicked();

    void on_refresh();

    void on_clearButton_pressed();

    void on_contextSlider_valueChanged(int value);

    void on_responseSlider_valueChanged(int value);

signals:
    void refresh();

private:
    Ui::MainWindow *ui;

    Chat chat;
    LlamaThread *llamaThread = NULL;
    bool generating = false;

    float decimalMultiplier = 100.f;

    void startGenerating();
    void stopGenerating();
    void refreshChat();
    void updateSliderValues();
};
#endif // MAINWINDOW_H
