#include "classifierengine.h"
#include <QtMath>
#include <QtDebug>
#include <QtConcurrent>

ClassifierEngine::ClassifierEngine(QVector<Element> &baseElements)
{
    qDebug() << "--------K-NN Training--------";

    normalizeFeatures(baseElements);

    for(int i = 0; i < baseElements.size(); i++)
    {
        unsigned char label = baseElements[i].getLabel();

        if(!(classes.keys().contains(label)))
        {
            classes.insert(label, Class(label));
        }
        classes[label].addBaseElement(baseElements[i]);
    }

    qDebug() << "Created" << classes.size() << "classes";
}

float ClassifierEngine::distance(Element from, Element to)
{
    QVector<float> features1 = from.getFeatures();
    QVector<float> features2 = to.getFeatures();

    int test = qMin(features1.size(), features2.size());

    float sum = 0.0f;

    if(t >= 1)
    {
        for(int i = 0; i < test; i++)
        {
            float x_y = features1[i] - features2[i];

            float pow = x_y;
            for(int p = 1; p < t; p++)
            {
                pow *= x_y;
            }

            sum += pow;
        }
        if(t>=2)
            sum = qPow(sum, 1.0f/t);
        if(sum < 0)
            sum = -sum;
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

void ClassifierEngine::classify(QVector<Element> &elements, int task_length)
{
    qDebug() << "--------K-NN Classifier--------";

    normalizeFeatures(elements);

    qDebug() << "Separating task into threads";

    for(int n = 0; n < elements.size(); n += task_length)
    {
        int len = n + task_length >= elements.size()?  elements.size()-n : task_length;
        QtConcurrent::run(this, &ClassifierEngine::classifyThread, std::ref(elements), n, len);
    }

    qDebug() << "Wait for tasks";
    QThreadPool::globalInstance()->waitForDone();

    for(int k = 0; k < classes.size(); k++)
    {
        unsigned char label = classes.keys()[k];
        qDebug() << "Class" << (char)label
                 << "Percent" << classes[label].goods()*100.0f / (classes[label].goods() + classes[label].bads())
                 << "Elements" << classes[label].getElements().size()
//                 << "Good" << classes[label].goods() << "Bad" << classes[label].bads()
                ;
    }
    qDebug() << "Step finished\n";

    qDebug() << "Correctly classified"   << good*100.0f/(good+bad) << "Percent";
//    qDebug() << "Incorrectly classified" << bad  << "elements";
}

void ClassifierEngine::classifyThread(QVector<Element> &elements, int n, int len)
{
    for(int i = n; i < n+len; i++)
    {
        Element element = elements[i];
        QMap<float, unsigned char> distances;

        for(int k = 0; k < classes.count(); k++)
        {
            unsigned char label = classes.keys()[k];
            QVector<Element> baseElements = classes[label].getBaseElement();

            for(int n = 0; n < baseElements.size(); n++)
            {
                float dist = distance(element, baseElements[n]);
                distances.insert(dist, label);
            }
        }

        QList<unsigned char> vals = distances.values().mid(0,K);

        qSort(vals);
        unsigned char final;
        int occurences = 0;

        for(int k = 0; k < classes.size(); k++)
        {
            unsigned char label = classes.keys()[k];

            int classCount = vals.count(label);

//            qDebug() << classCount << vals.size() << distances.size();

            if(classCount >= occurences)
            {
                occurences = classCount;
                final = label;
            }
        }
//        qDebug() << element.getLabel() << final << occurences << K;
        QMutexLocker locker(&mutex);
        classes[final].addElement(element);
        if(final == element.getLabel())
        {
            good++;
            classes[final].isGood();
        }
        else
        {
            bad++;
            classes[final].isBad();
        }
        locker.unlock();
    }
}

void ClassifierEngine::normalizeFeatures(QVector<Element> &elements)
{

    if(mins.isEmpty() && maxes.isEmpty())
    {
        qDebug() << "Finding features mins and maxes";

        for(int i = 0; i < elements.size(); i++)
        {
            QVector<float> features = elements[i].getFeatures();
            for (int j = 0; j < features.size(); j++)
            {
                if(j >= mins.size())
                {
                    mins .push_back(features[j]);
                    maxes.push_back(features[j]);
                }
                else
                {
                    float val = features[j];

                    mins[j]  = qMin(mins [j], val);
                    maxes[j] = qMax(maxes[j], val);
                }
            }
        }

        for(int i = 0; i < maxes.size(); i++)
        {
            qDebug() << "Feature:" << i << "Min:" << mins[i] << "Max:" << maxes[i] << "Delta:" << maxes[i] - mins[i];

            float maxmin   = maxes[i] - mins[i];
            maxes[i] = maxmin != 0.0f ? 1.0f / maxmin : 0.0f;
        }

    }

    qDebug() << "Normalizing" << elements.size() << "elements";

    for(int i = 0; i < elements.size(); i++)
    {
        QVector<float> features = elements[i].getFeatures();

        for (int j = 0; j < features.size(); j++)
        {
            features [j] = ( features[j] - mins[j] ) * maxes[j];
        }
        elements[i].setFeatures(features);
    }

    qDebug() << "Task done";
}
