#ifndef PATTERNCLASSIFIER_H
#define PATTERNCLASSIFIER_H

#include "classifier.h"
#include <QtMath>



class BayesClassifier : public Classifier
{
public:
    BayesClassifier();

protected:

    QMap<QString,QVector<QMap<double,double>>> densities;

    double kernel(double x);

    double sqrt2pi = 1.0/qSqrt(2*M_PI);

    // Classifier interface
public slots:
    void process(Element *element, int x, int y, int step);

    // Classifier interface
public slots:
    void setClassElements(QMap<QString, Class *> classes, QVector<Element *> elements);
};

#endif // PATTERNCLASSIFIER_H
