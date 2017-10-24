#include <QCoreApplication>
#include <QtDebug>
#include "io.h"

int main(int argc, char *argv[])
{
    QVector<digit> digits = io::parse("..\\train-labels.idx1-ubyte","..\\train-images.idx3-ubyte");

//    io::deparse(digits, "digit");

    return 0;
}
