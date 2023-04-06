#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(w.width(),w.height());
    w.setWindowTitle("Tomasulo-based RISC-V simulator");
    w.show();
    return a.exec();
}
