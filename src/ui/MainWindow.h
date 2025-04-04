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
#warning no exceptions in event handlers


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
    void on_appendText();
    void on_updateGUI(Chat::chatStateE state);
    void on_exceptionOccured(QString errorMsg);

    void on_submitButton_clicked();

    void on_tempSlider_valueChanged(int value);

    void on_topKSlider_valueChanged(int value);

    void on_topPSlider_valueChanged(int value);

    void on_minPSlider_valueChanged(int value);

    void on_loadButton_clicked();

    void on_clearButton_pressed();

    void on_contextSlider_valueChanged(int value);

    void on_responseSlider_valueChanged(int value);

signals:
    void sendMessage(const QString &message, SamplersArrayT samplers);
    void interruptGeneration();
private:
    Ui::MainWindow *ui;

    ConfigApp config = ConfigApp("config.json");

    Chat chat;
    bool generating = false;

    float decimalMultiplier = 100.f;

    void procStateNotLoaded();
    void procStateLoading();
    void procStateIdle();
    void procStateGenerating();


    void startGenerating();
    void stopGenerating();
    void refreshChat();
    void initSliderValues() const;
    SamplersArrayT fetchSamplers() const;
};
#endif // MAINWINDOW_H
