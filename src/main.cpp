#include "ui/MainWindow.h"

#include <QApplication>
#include <QMessageBox>

#define ERROR_HEADER "Error"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    int result;
    w.show();
    try {
        result = a.exec();
    }
    catch (std::exception &e) { // this should never happen, but it's here in case shit hits the fan
        QMessageBox::critical(0, ERROR_HEADER, e.what());
        result = EXIT_FAILURE;
    }
    return result;
}
