#include "mainwindow.h"

#include <QApplication>
#include <QtWidgets>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    shitmap shit;
    shit.setSourceModel(createMailModel(&shit));
    shit.resize(1000,1000);
    shit.show();
    return a.exec();
}
