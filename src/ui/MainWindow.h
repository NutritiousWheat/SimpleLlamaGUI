#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "../config/ConfigApp.h"
#include "../chat/Chat.h"
#include "../llm/LlamaThread.h"

#warning TODO
#warning use QThread for llamathread
#warning use QObject for the rest of the classes
#warning do signals and slots

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

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

    ConfigApp config = ConfigApp("config.json");

    Chat chat;
    LlamaThread *llamaThread = nullptr;
    bool generating = false;

    float decimalMultiplier = 100.f;

    void startGenerating();
    void stopGenerating();
    void refreshChat();
    void updateSliderValues();
};
#endif // MAINWINDOW_H
