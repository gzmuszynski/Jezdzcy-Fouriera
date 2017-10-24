#ifndef IO_H
#define IO_H

#include "digit.h"

#include <QVector>

// class responsible for handling IO file operations

class io
{
public:
    static QVector<digit> parse      (QString labels, QString pictures);
    static void           serialize  (QVector<digit>, QString filename);
    static QVector<digit> deserialize(QString filename);

};

#endif // IO_H
