#include <QApplication>
#include "entrance.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Entrance w;
    w.show();
    return a.exec();
}
