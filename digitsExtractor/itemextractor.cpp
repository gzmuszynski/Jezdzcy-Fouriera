#include <QCoreApplication>
#include <QtDebug>
#include "io.h"
#include "starextractor.h"




int main(int argc, char *argv[])
{
    qDebug() << "-----------------------";
    qDebug() << "STAR features extractor";
    qDebug() << "-----------------------";

    QVector<QVector<Element>> elements;

    QString pictures;

    QString text_out;

    for(int i = 1; i < argc; i++)
    {
        std::string test = argv[i];

        if(test == "-f") // input files
        {
            pictures   = argv[i+1];

            qDebug() << "Input folder" << pictures;

            i+=1;
        }
        if(test == "-t") // save features
        {
            text_out = argv[i+1];

            qDebug() << "Text output" << text_out;

            i+=1;
        }
    }
    if(!(pictures.isEmpty()))
    {
        qDebug() << "";
        elements = io::parseSTAR(pictures);

        STARExtractor::extract(std::ref(elements));
        if(!(text_out.isEmpty()))
        {
            io::serialize(elements[0], text_out + "starTrain.txt");
            io::serialize(elements[1], text_out + "starPlain.txt");
            io::serialize(elements[2], text_out + "starLight.txt");
            io::serialize(elements[3], text_out + "starLight30.txt");
        }
    }

    return 0;
}
