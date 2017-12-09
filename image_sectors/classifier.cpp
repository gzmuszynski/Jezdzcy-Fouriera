#include "classifier.h"
#include <QtMath>

Classifier::Classifier()
{

}

void Classifier::setMetric(int value)
{
    metric = value;
}

void Classifier::normalizeClasses()
{


    QVector<double> features = elements[0]->getFeatures();

    for(int i = 0; i < features.size(); i++)
    {
        mins. push_back(features[i]);
        maxes.push_back(features[i]);
    }
    for(Element* el:elements)
    {
        QVector<double> features = el->getFeatures();
        for(int i = 0; i < features.size(); i++)
        {
            mins [i] = qMin(mins[i],  features[i]);
            maxes[i] = qMax(maxes[i], features[i]);
        }
    }
    for(Element* el:elements)
    {
        QVector<double> features = el->getFeatures();
        for(int i = 0; i < features.size(); i++)
        {
            features[i] = (features[i] - mins[i]) / (maxes[i] - mins[i]);
        }
        el->setFeatures(features);
    }
}

void Classifier::normalize(Element *element)
{
    QVector<double> features = element->getFeatures();
    for(int i = 0; i < features.size(); i++)
    {
        features[i] = (features[i] - mins[i]) / (maxes[i] - mins[i]);
    }
    element->setFeatures(features);
}

double Classifier::distance(Element *from, Element *to)
{
    QVector<double> features1 = from->getFeatures();
    QVector<double> features2 = to->getFeatures();

    int test = qMin(features1.size(), features2.size());

    double sum = 0.0f;

    if(metric >= 1)
    {
        for(int i = 0; i < test; i++)
        {
            double x_y = features1[i] - features2[i];

            if(x_y < 0)
                x_y = -x_y;

            double pow = x_y;

            for(int p = 1; p < metric; p++)
            {
                pow *= x_y;
            }

            sum += pow;
        }
        if(metric>=2)
            sum = qPow(sum, 1.0/metric);
    }
    if(metric == 0)
    {
        for(int i = 0; i < test; i++)
        {
            sum = qMax(sum,(features1[i] - features2[i]));
        }
    }
    return sum;
}
