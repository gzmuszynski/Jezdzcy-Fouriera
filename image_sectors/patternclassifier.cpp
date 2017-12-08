#include "patternclassifier.h"

PatternClassifier::PatternClassifier()
{

}

void PatternClassifier::process(Element *element, int x, int y, int step)
{
    normalize(element);

    QMap<double, QString> distances;

    for(Element* pivot:elements)
    {
        double dist = distance(pivot,element);
        distances.insert(dist, pivot->getLabel());
    }
    QString nearest = distances.values()[0];

    emit classified(nearest, x, y, step);
}

void PatternClassifier::setClassElements(QMap<QString, Class *> classes, QVector<Element *> elements)
{
    QMap<QString, Class*> newClasses;
    for(Class* clss:classes.values())
    {
        QString label = clss->getLabel();

        QVector<Element*> elems = clss->getElements();
        double sizeDiv = 1.0/(elems.size());

        QVector<double> features(elems[0]->getFeatures().size());

        // Pivot

        for(Element* elem:elems)
        {
            QVector<double> f = elem->getFeatures();
            for(int i = 0; i < features.size(); i++)
            {
                features[i] += f[i];
            }
        }
        for(double f:features)
        {
            f*=sizeDiv;
        }
        Element* pivot = new Element(label,features);

        // Radius

        double radius;

        for(Element* elem:elems)
        {
            double dist = distance(pivot,elem);
            radius = qMax(radius, dist);
        }

        // Set up class

        Class* newClass = new Class(label, pivot, radius);

        newClasses[label] = newClass;
        elements.push_back(pivot);
    }

    this->classes = newClasses;
    normalizeClasses();
}
