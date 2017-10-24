#include "io.h"

#include <QFile>
#include <QtDebug>
#include <QtEndian>


QVector<digit> io::parse(QString labels, QString pictures)
{
    QVector<digit> digits;

    QFile labelsFile  (labels);
    QFile picturesFile(pictures);


    labelsFile.  open(QFile::ReadOnly);
    picturesFile.open(QFile::ReadOnly);

    QDataStream labelsData  (&labelsFile);
    QDataStream picturesData(&picturesFile);

    labelsData.  setByteOrder(QDataStream::BigEndian);
    picturesData.setByteOrder(QDataStream::BigEndian);

    qDebug() << "Files" << labelsFile.fileName() << picturesFile.fileName();

    if(labelsFile.isReadable() && picturesFile.isReadable())
    {
        int32_t magicNumber_l, magicNumber_p;
        int32_t numItems_l,    numItems_p;
        int32_t height, width;

        qDebug() << "Files set up";

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

            digits.push_back(digit(label, picture));
        }
        qDebug() << "Parsed items count" << digits.size();
    }
    else
    {
        qDebug() << "File statuses" << labelsFile.isOpen() << picturesFile.isOpen();
    }

    return digits;
}

void io::deparse(QVector<digit> digits, QString filename)
{
    qDebug() << "Save pictures destination" << QString("..\\images\\%1_NUMBER.bmp").arg(filename);
    for(int i = 0; i < digits.size(); i++)
    {
        QImage picture = digits[i].getPicture();
        picture.save(QString("..\\images\\%1_%2.bmp").arg(filename).arg(i, 5, 10, QChar('0')),"bmp");
    }
    qDebug() << "Saved files count" << digits.size();
}

void io::serialize(QVector<digit> digits, QString filename)
{

}

QVector<digit> io::deserialize(QString filename)
{
    return QVector<digit>();
}
