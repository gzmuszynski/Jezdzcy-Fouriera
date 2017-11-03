#ifndef STAREXTRACTOR_H
#define STAREXTRACTOR_H
#include "element.h"
#include <QVector>

class STARExtractor
{
public:
    static void extract      (QVector<QVector<Element>> &elements);
    static void extractThread(Element &element);
};

#endif // STAREXTRACTOR_H
