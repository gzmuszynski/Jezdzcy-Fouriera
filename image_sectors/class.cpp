#include "class.h"

Class::Class(QString label) : label(label)
{

}

Class::Class(QString label, Element *pivot, double radius)
    : label(label),
      radius(radius)
{
    elements.push_back(pivot);
}

QVector<Element *> Class::getElements() const
{
    return elements;
}

void Class::addElement(Element *value)
{
    elements.push_back(value);
}

double Class::getRadius() const
{
    return radius;
}

QString Class::getLabel() const
{
    return label;
}
