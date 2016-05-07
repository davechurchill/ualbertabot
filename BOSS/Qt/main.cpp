#include "mainwindow.h"
#include <QApplication>

#include "BOSS.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));

    QCoreApplication::addLibraryPath("./");
    BOSS::init();

    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("StarCraft Build-Order Visualization");
    w.show();

    return a.exec();
}
