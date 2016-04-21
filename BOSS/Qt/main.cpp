#include "mainwindow.h"
#include <QApplication>

#include "BOSS.h"

int main(int argc, char *argv[])
{
    BOSS::init();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
