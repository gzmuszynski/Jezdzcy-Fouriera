#ifndef CLASS_H
#define CLASS_H
#include "element.h"

class Class{
public:
    Class()                                         { }
    Class(unsigned char label) : label(label)       { }

    void           setLabel(unsigned char label)    { this->label = label; }
    unsigned char  getLabel()                       { return label; }

    void           addElement    (Element &element) { elements.push_back(element); }
    QVector<Element>&getElements()                  { return elements; }
    void           addBaseElement(Element &element) { baseElements.push_back(element); }
    QVector<Element>&getBaseElement()               { return baseElements; }

    void isGood()                                   { good++; }
    void isBad()                                    { bad++; }
    int  goods()                                    { return good; }
    int  bads()                                     { return bad; }

private:
    unsigned char  label;

    QVector<Element> baseElements;
    QVector<Element> elements;

    int good;
    int bad;
};

#endif // CLASS_H
