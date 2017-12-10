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
    this->elements = QVector<Element*>(elements);
    this->classes = QMap<QString, Class *>(classes);

    normalizeClasses();
}
