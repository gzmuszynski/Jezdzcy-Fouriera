#ifndef KNEARESTNEIGHBOURCLASSIFIER_H
#define KNEARESTNEIGHBOURCLASSIFIER_H

#include "classifier.h"



class KNearestNeighbourClassifier : public Classifier
{
public:
    KNearestNeighbourClassifier();

    void setK(int value);

private:
    int k;

    // Classifier interface
public slots:
    void process(Element *element, int x, int y, int step);

    void setClassElements(QMap<QString,Class*> classes, QVector<Element*> elements);
};

#endif // KNEARESTNEIGHBOURCLASSIFIER_H
