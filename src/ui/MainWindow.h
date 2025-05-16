#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>

#include "../config/ConfigApp.h"
#include "../chat/Chat.h"
#include "../llm/LlamaThread.h"

#define CRIT_HEADER "Critical error"
#define ERR_HEADER "Error"

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
    void on_appendText(QString &text);
    void on_setText(QString &text);
    void on_updateGUI(Chat::chatStateE state);
    void on_exceptionOccured(QString errorMsg);

    void on_submitButton_clicked();

    void on_continueButton_clicked();

    void on_loadButton_clicked();

    void on_clearButton_pressed();

    void on_unloadButton_clicked();

    void on_refreshButton_clicked();


public slots:
    void on_sampler_valueChanged(Sampler sampler);

private:
    QSlider test;
    Ui::MainWindow *ui;

#ifdef __APPLE__
    // TODO: get path relative to .app... what in the world is an .app file????
    ConfigApp config = ConfigApp("/Users/potato/code_projects/llm_ui/cmake-build-debug/config.json");
#else
    ConfigApp config = ConfigApp("config.json");
#endif

    Chat chat;
    SamplerArray samplers;



    void procStateNotLoaded();
    void procStateLoading();
    void procStateIdle();
    void procStateGenerating();


    void startGenerating();
    void stopGenerating();
    void refreshModels();
};
#endif // MAINWINDOW_H
