#include "classifier.h"

#include <QColor>
#include <QtDebug>

Classifier::Classifier(QObject *parent)
{

}

QMap<unsigned int,QVector<Element*>> Classifier::createGroupsAndClassify(QVector<Element *> elements)
{
    QMap<unsigned int,QVector<Element*>> classes;

    double maxd = 0;
    Element* max1;
    Element* max2;
    for(Element* e1: elements)
    {
        for(Element* e2: elements)
        {
            double d = distance(e1,e2);
            if(d > maxd)
            {
                maxd = d;
                max1 = e1;
                max2 = e2;
            }
        }
    }
    unsigned int rgb1 = QColor(max1->getR(),max1->getG(),max1->getB()).rgba();
    unsigned int rgb2 = QColor(max2->getR(),max2->getG(),max2->getB()).rgba();

    classes[rgb1].push_back(max1);
    classes[rgb2].push_back(max2);

    for(Element* e1: elements)
    {
        double dist1 = distance(max1,e1);

        for(Element* e2 : classes[rgb1])
        {
            dist1 = qMin(dist1, distance(e1,e2));
        }
        double dist2 = distance(max2,e1);

        for(Element* e2 : classes[rgb2])
        {
            dist2 = qMin(dist2, distance(e1,e2));
        }

        if(dist1 == 0 || dist2 == 0)
        {

        }
        else if(dist1<dist2)
        {
            classes[rgb1].push_back(e1);
        }
        else
        {
            classes[rgb2].push_back(e1);
        }
    }

    for(unsigned int rgb : classes.keys())
//    qDebug() << rgb << classes[rgb].size();

    return classes;
}

double Classifier::distance(Element *e1, Element *e2)
{
    double r1 = (e2->getR()-e1->getR())*0.003921568627451;
    double g1 = (e2->getG()-e1->getG())*0.003921568627451;
    double b1 = (e2->getB()-e1->getB())*0.003921568627451;

    return r1*r1+g1*g1;
}
