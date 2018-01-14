#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "element.h"

#include <QMap>
#include <QObject>
#include <QVector>
#include <QColor>



class Classifier : public QObject
{
    Q_OBJECT
public:
    explicit Classifier(QObject* parent = nullptr);

    QMap<unsigned,QVector<Element*>> createGroupsAndClassify(QVector<Element*> elements);

private:
    double distance(Element* e1, Element* e2);

};

#endif // CLASSIFIER_H
