#include "io.h"

#include <QFile>
#include <QtDebug>
#include <QtEndian>


QVector<Element> io::parse(QString labels, QString pictures)
{
    qDebug() << "--------MINST Parser--------";

    QVector<Element> digits;

    QFile labelsFile  (labels);
    QFile picturesFile(pictures);

    labelsFile.  open(QFile::ReadOnly);
    picturesFile.open(QFile::ReadOnly);

    QDataStream labelsData  (&labelsFile);
    QDataStream picturesData(&picturesFile);

    labelsData.  setByteOrder(QDataStream::BigEndian);
    picturesData.setByteOrder(QDataStream::BigEndian);

    if(labelsFile.isReadable() && picturesFile.isReadable())
    {
        int32_t magicNumber_l, magicNumber_p;
        int32_t numItems_l,    numItems_p;
        int32_t height, width;

        qDebug() << "Files set up\nParsing Files";

        labelsData   >> magicNumber_l;
        picturesData >> magicNumber_p;

        qDebug() << "Magic numbers" << magicNumber_l << magicNumber_p;

        if(magicNumber_l != 2049 || magicNumber_p != 2051) // are files correct?
        {
            return digits; // HIGHLY UNSTABLE
        }

        labelsData   >> numItems_l;
        picturesData >> numItems_p;

        qDebug() << "Number of items" << numItems_l << numItems_p;

        if(numItems_l != numItems_p) // are the sets pairable?
        {
            return digits; // HIGHLY UNSTABLE
        }

        picturesData >> width;
        picturesData >> height;

        qDebug() << "Resolution" << width << height;

        for(int i = 0; i < numItems_l; i++)
        {
            unsigned char label;
            QImage picture(width, height, QImage::Format_Grayscale8);

            labelsData   >> label;
            for(int x = 0; x < width; x++)
            {
                for(int y = 0; y < height; y++)
                {
                    unsigned char pixel;
                    picturesData >> pixel;
                    pixel= 255 - pixel;

                    picture.setPixelColor(y,x,QColor(pixel,pixel,pixel));
                }
            }

            digits.push_back(Element(label, picture));
        }
        qDebug() << "Parsed items count" << digits.size();
    }
    else
    {
        qDebug() << "File statuses" << labelsFile.isOpen() << picturesFile.isOpen();
    }

    qDebug() << "Step finished\n";
    return digits;
}

void io::deparse(QVector<Element> digits, QString filename)
{
    qDebug() << "--------Picture Extractor--------";
    qDebug() << "Save pictures destination" << QString("%1_NUMBER.bmp").arg(filename);

    for(int i = 0; i < digits.size(); i++)
    {
        QImage picture = digits[i].getPicture();
        picture.save(QString("%1_%2.bmp").arg(filename).arg(i, 5, 10, QChar('0')),"bmp");
    }

    qDebug() << "Saved files count" << digits.size();
    qDebug() << "Step finished\n";
}

void io::serialize(QVector<Element> digits, QString filename)
{
    qDebug() << "--------Serializer--------";

    QFile file(filename);
    qDebug() << "Creating file" << filename;

    if( file.open(QFile::WriteOnly))
    {
        qDebug() << "Saving features to file";
        QTextStream out(&file);

        for(int i = 0; i < digits.size(); i++)
        {
            if(i > 0)
                out << "\n";

            out << digits[i].getLabel();

            QVector<float> features = digits[i].getFeatures();
            for(int n = 0; n < features.size(); n++)
            {
                out << " " << features[n];
            }

        }
    }
    qDebug() << "Saved objects count" << digits.size();
    file.close();

    qDebug() << "Step finished\n";
}

QVector<Element> io::deserialize(QString filename)
{
    qDebug() << "--------Deserializer--------";

    QFile file(filename);
    qDebug() << "Opening file" << filename;

    QVector<Element> digits;

    if( file.open(QFile::ReadOnly))
    {
        qDebug() << "Reading features from file";
        QTextStream in(&file);

        int i = 0;
        while(!(in.atEnd()))
        {
            QString       line  = in.readLine();
            QStringList   list  = line.split(' ');

            unsigned char label = list[0].toStdString().c_str()[0];
            QVector<float> features;

            for(int n = 1; n < list.size(); n++)
            {
                features.push_back(list[n].toFloat());
            }

            digits.push_back(Element(label, features));

            i++;
        }
    }
    qDebug() << "Read objects count" << digits.size();
    file.close();

    qDebug() << "Step finished\n";
    return digits;
}
