#include "counterwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CounterWindow w;
    w.show();

    return a.exec();
}
