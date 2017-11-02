#ifndef FEATUREEXTRACTOR_H
#define FEATUREEXTRACTOR_H
#include "element.h"
#include <QVector>

class MINSTExtractor
{
public:
    static void extract      (QVector<Element> &digits, int task_length);
    static void extractThread(QVector<Element> &digits, int n, int len);
};

#endif // FEATUREEXTRACTOR_H
