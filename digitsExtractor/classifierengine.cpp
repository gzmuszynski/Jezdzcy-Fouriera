#include "classifierengine.h"
#include <QtMath>

ClassifierEngine::ClassifierEngine(QVector<digit> &baseElements)
{

}

void ClassifierEngine::classify(QVector<digit> &elements)
{

}

float ClassifierEngine::distance(digit from, digit to)
{
    QVector<float> features1 = from.getFeatures();
    QVector<float> features2 = to.getFeatures();

    int test = std::min(features1.size(), features2.size());

    float sum = 0.0f;

    if(t >= 1)
    {
        for(int i = 0; i < test; i++)
        {
            float x_y = features1[i] - features2[i];

            float pow = 0.0f;
            for(int p = 0; p < t; p++)
            {
                pow *= x_y;
            }

            sum += pow;
        }
        if(t>=2)
            sum = qPow(sum, 1.0f/t);
    }
    if(t == 0)
    {
        for(int i = 0; i < test; i++)
        {
            sum = qMax(sum,(features1[i] - features2[i]));
        }
    }
    return sum;
}

void ClassifierEngine::classifyThread(QVector<digit> &elements, int n, int len)
{


}
