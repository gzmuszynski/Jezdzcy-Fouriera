#ifndef IO_H
#define IO_H

#include "element.h"

#include <QVector>

// class responsible for handling IO file operations

class io
{
public:
    static QVector<Element> parse       (QString labels, QString pictures);
    static void           deparse     (QVector<Element> digits, QString filename);
    static void           serialize   (QVector<Element> digits, QString filename);
    static QVector<Element> deserialize (QString filename);

};

#endif // IO_H
