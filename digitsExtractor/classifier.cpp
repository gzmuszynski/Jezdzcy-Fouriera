#include <QCoreApplication>
#include <QtDebug>
#include "featureextractor.h"
#include "io.h"




int main(int argc, char *argv[])
{
    qDebug() << "---------------";
    qDebug() << "k-NN Classifier";
    qDebug() << "---------------";

    QVector<digit> digitsTrain;
    QVector<digit> digits;

    QString filename;
    QString filenameTrain;

    int K = 25;
    int task_length = 100;

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
        if(test == "-k") // save pictures
        {
            QString stringK = argv[i+1];
            k = stringK.toInt();

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
    }
    if(!(filename.isEmpty()) && !(imagesFilename.isEmpty()))
    {
        qDebug() << "";

        digitsTrain = io::deserialize(filenameTrain);
        digits      = io::deserialize(filename);

    }

    return 0;
}
