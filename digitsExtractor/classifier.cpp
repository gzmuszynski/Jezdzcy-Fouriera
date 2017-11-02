#include <QCoreApplication>
#include <QtDebug>
#include "classifierengine.h"
#include "featureextractor.h"
#include "io.h"




int main(int argc, char *argv[])
{
    qDebug() << "---------------";
    qDebug() << "k-NN Classifier";
    qDebug() << "---------------";

    QVector<Element> elementsTrain;
    QVector<Element> elements;

    QString filename;
    QString filenameTrain;

    int K = 25;
    int task_length = 100;
    int metric = 1;

    for(int i = 1; i < argc; i++)
    {
        std::string test = argv[i];

        if(test == "-f") // input files
        {
            filenameTrain   = argv[i+1];
            filename        = argv[i+2];

            qDebug() << "Input files" << filenameTrain << filename;

            i+=3;
        }
        if(test == "-k") // K number
        {
            QString stringK = argv[i+1];
            K = stringK.toInt();

            qDebug() << "K" << K;

            i+=1;
        }
        if(test == "-l") // task lenght
        {
            QString stringTaskLength = argv[i+1];
            task_length = stringTaskLength.toInt();

            qDebug() << "Thread task lenght" << task_length;

            i+=1;
        }
        if(test == "-t") // task lenght
        {
            QString stringMetricLength = argv[i+1];
            metric = stringMetricLength.toInt();

            qDebug() << "Metric parameter" << metric;

            i+=1;
        }
    }
    if(!(filename.isEmpty()) && !(filenameTrain.isEmpty()))
    {
        qDebug() << "";

        elementsTrain = io::deserialize(filenameTrain);
        elements      = io::deserialize(filename);

        ClassifierEngine engine(elementsTrain);

        engine.setK(K);
        engine.setDistanceParameter(metric);
        engine.classify(elements, task_length);

    }


    return 0;
}
