#include <QCoreApplication>
#include <QtDebug>
#include "featureextractor.h"
#include "io.h"




int main(int argc, char *argv[])
{
    qDebug() << "------------------------";
    qDebug() << "MINST features extractor";
    qDebug() << "------------------------";

    QVector<digit> digits;

    QString labels;
    QString pictures;

    QString pictures_out;
    QString text_out;

    int task_length = 100;

    for(int i = 1; i < argc; i++)
    {
        std::string test = argv[i];

        if(test == "-f") // input files
        {
            labels   = argv[i+1];
            pictures = argv[i+2];

            qDebug() << "Input files" << labels << pictures;

            i+=2;
        }
        if(test == "-p") // save pictures
        {
            pictures_out = argv[i+1];

            qDebug() << "Pictures output" << pictures_out;

            i+=1;
        }
        if(test == "-t") // save features
        {
            text_out = argv[i+1];

            qDebug() << "Text output" << text_out;

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
    if(!(labels.isEmpty()) && !(pictures.isEmpty()))
    {
        qDebug() << "";
        digits = io::parse(labels, pictures);


        if(!(pictures_out.isEmpty()))
        {
            io::deparse(digits, pictures_out);
        }
        else
        {
            FeatureExtractor::extract(std::ref(digits), task_length);
        }
        if(!(text_out.isEmpty()))
        {
            io::serialize(digits, text_out);
        }
    }

    return 0;
}
