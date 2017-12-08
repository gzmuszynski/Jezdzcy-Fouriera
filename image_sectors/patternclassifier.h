#ifndef PATTERNCLASSIFIER_H
#define PATTERNCLASSIFIER_H

#include "classifier.h"



class PatternClassifier : public Classifier
{
public:
    PatternClassifier();

protected:

    // Classifier interface
public slots:
    void process(Element *element, int x, int y, int step);

    // Classifier interface
public slots:
    void setClassElements(QMap<QString, Class *> classes, QVector<Element *> elements);
};

#endif // PATTERNCLASSIFIER_H
