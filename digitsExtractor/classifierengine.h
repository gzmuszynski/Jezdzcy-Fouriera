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
    void setDistanceParameter(int t) { this.t = t; }

private:
    void classifyThread(QVector<digit> &elements, int n, int len);

    QVector<Class> classes;
    int t = 1;
};

#endif // CLASSIFIERENGINE_H
