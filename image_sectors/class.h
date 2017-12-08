#ifndef CLASS_H
#define CLASS_H
#include <QString>
#include <element.h>

class Class
{
public:
    Class(QString label);
    Class(QString label,Element* pivot, double radius);

    QString getLabel() const;
    QVector<Element *> getElements() const;
    void addElement(Element *value);

    double getRadius() const;

private:
    QString label;
    QVector<Element*> elements;
    double radius;
};

#endif // CLASS_H
