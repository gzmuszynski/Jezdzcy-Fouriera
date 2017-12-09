#include "knearestneighbourclassifier.h"
#include <QtDebug>

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
    QMap<int, QString> final;
    for(QString clss:classes.keys())
    {
        final[classes[clss]] = clss;
    }

    QString className = final.last();

//    int rand = qrand() % 5;
//    qDebug() << nearest[0]->getLabel() << " " << distances.keys()[0];
    emit classified(className,x,y,step);
}

void KNearestNeighbourClassifier::setClassElements(QMap<QString, Class *> classes, QVector<Element*> elements)
{
    this->elements = elements;
    this->classes = classes;

    normalizeClasses();
}
