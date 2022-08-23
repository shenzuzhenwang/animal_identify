#include "Animal.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Animal w;
    w.show();
    return a.exec();
}
