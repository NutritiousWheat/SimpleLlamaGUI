#include "ui/MainWindow.h"

#include <QApplication>
#include <QMessageBox>






int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    int result;

    w.show();

    try {
        result = a.exec();
    }
    catch (std::exception &e) { // this should never happen, but it would be nice to show an error if something goes really wrong
        QMessageBox::critical(nullptr, CRIT_HEADER, e.what());
        result = EXIT_FAILURE;
    }
    return result;
}
