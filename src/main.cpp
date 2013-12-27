/*
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#include "ui/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
