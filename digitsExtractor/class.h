#ifndef CLASS_H
#define CLASS_H
#include "digit.h"

class Class{
public:
    Class()                                       { }
    Class(unsigned char label) : label(label)     { }

    void           setLabel(unsigned char label)  { this->label = label; }
    unsigned char  getLabel()                     { return label; }

    void           addElement    (digit &element) { elements.push_back(element); }
    QVector<digit> getElements()                  { return elements; }
    void           addBaseElement(digit &element) { baseElements.push_back(element); }
    QVector<digit> getBaseElement()               { return baseElements; }

private:
    unsigned char  label;

    QVector<digit> baseElements;
    QVector<digit> elements;
};

#endif // CLASS_H
