#ifndef CLASSIFIERENGINE_H
#define CLASSIFIERENGINE_H

#include "class.h"
#include "digit.h"



class ClassifierEngine
{
public:
    ClassifierEngine(QVector<digit> &baseElements);

    void classify(QVector<digit> &elements);

    float distance(digit from, digit to);

private:
    void classifyThread(QVector<digit> &elements, int n, int len);

    QVector<Class> classes;
};

#endif // CLASSIFIERENGINE_H
