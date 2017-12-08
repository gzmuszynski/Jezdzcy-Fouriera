#include "knearestneighbourclassifier.h"

KNearestNeighbourClassifier::KNearestNeighbourClassifier()
{
    k = 2;
}

void KNearestNeighbourClassifier::setK(int value)
{
    k = value;
}

void KNearestNeighbourClassifier::process(Element *element, int x, int y, int step)
{
    normalize(element);

    QMap<double,Element*> distances;
    for(Element* comparedElement:elements)
    {
        double dist = distance(element,comparedElement);

        distances[dist] = comparedElement;
    }

    QList<Element*> nearest = distances.values().mid(0, k);

    QMap<QString, int> classes;

    for(Element* near:nearest)
    {
        QString className = near->getLabel();
        if(classes.contains(className))
            classes[className] ++;
        else
            classes[className] = 1;
    }

    QString className = classes.firstKey();

//    int rand = qrand() % 5;

    emit classified(className,x,y,step);
}

void KNearestNeighbourClassifier::setClassElements(QMap<QString, Class *> classes, QVector<Element*> elements)
{
    this->elements = elements;
    this->classes = classes;

    normalizeClasses();
}
