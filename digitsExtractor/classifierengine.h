#ifndef CLASSIFIERENGINE_H
#define CLASSIFIERENGINE_H

#include "class.h"
#include "element.h"

#include <QMap>
#include <QMutex>



class ClassifierEngine
{
public:
    ClassifierEngine(QVector<Element> &baseElements);

    void classify(QVector<Element> &elements, int task_length);

    void setDistanceParameter(int t) { this->t = t; }
    void setK(int k)                 { this->K = k; }

private:
    float distance(Element from, Element to);

    void classifyThread(QVector<Element> &elements, int n, int len);

    void normalizeFeatures(QVector<Element> &elements);

    QMap<unsigned char, Class> classes;
    int t = 1;
    int K = 25;

    int good = 0;
    int bad  = 0;

    mutable QMutex mutex;
    QVector<float> mins;
    QVector<float> maxes;
};

#endif // CLASSIFIERENGINE_H
