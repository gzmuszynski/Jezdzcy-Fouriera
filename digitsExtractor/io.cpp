#include "io.h"

#include <QDir>
#include <QFile>
#include <QtDebug>
#include <QtEndian>


QVector<Element> io::parseMINST(QString labels, QString pictures)
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
            label += 48;
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

QVector<QVector<Element>> io::parseSTAR(QString pictures)
{
    QMap<QString, unsigned char> STAR;
    STAR["brus"] = 'B'; STAR["clam"] = 'C';
    STAR["hook"] = 'H'; STAR["knif"] = 'K';
    STAR["plie"] = 'p'; STAR["penc"] = 'P';
    STAR["scis"] = 'S'; STAR["scre"] = 'R';
    STAR["span"] = 'N'; STAR["stri"] = 'T';


    qDebug() << "--------STAR Parser--------";

    QDir root            (pictures);
    QDir train           (pictures + "/train");
    QDir plain      (pictures + "/test_plain");
    QDir light      (pictures + "/test_light");
    QDir light30 (pictures + "/test_30st_light");

    qDebug() << "Directories set";

    QStringList plainFiles = plain.entryList(QDir::Files);
    QStringList lightFiles = light.entryList(QDir::Files);
    QStringList light30Files  = light30.entryList(QDir::Files);
    QStringList trainDirs  = train.entryList(QDir::Dirs);

    qDebug() << "File lists set";

    QVector<Element> plainVector;
    QVector<Element> lightVector;
    QVector<Element> light30Vector;
    QVector<Element> trainVector;

    for(int i = 0; i < plainFiles.size(); i++)
    {
        QString plainString   = plainFiles[i];
        QString lightString   = lightFiles[i];
        QString light30String = light30Files[i];

        QImage  plainImage(plain.path()+"/"+plainString);
        QImage  lightImage(light.path()+"/"+lightString);
        QImage  light30Image(light30.path()+"/"+light30String);

        Element plainElement(STAR[plainString.mid(0,4)], plainImage);
        Element lightElement(STAR[lightString.mid(0,4)], lightImage);
        Element light30Element(STAR[light30String.mid(0,4)], light30Image);

        plainVector.push_back(plainElement);
        lightVector.push_back(lightElement);
        light30Vector.push_back(light30Element);
    }

    qDebug() << "Test sets each contains" << plainVector.size() << lightVector.size() << light30Vector.size() << "elements";

    for(int i = 2; i < trainDirs.size(); i++)
    {
        train.cd(trainDirs[i]);
        unsigned char label = STAR[trainDirs[i].mid(0,4)];

        QStringList trainFiles = train.entryList(QDir::Files);

        for(int f = 0; f < trainFiles.size(); f++)
        {
            QString trainString = train.path()+"/"+trainFiles[f];
            QImage  trainImage(trainString);
            Element trainElement(label, trainImage);

            trainVector.push_back(trainElement);
        }
        train.cdUp();
    }

    qDebug() << "Train sets contains" << trainVector.size() << "elements";

    QVector<QVector<Element>> vector;

    vector.push_back(trainVector);

    vector.push_back(plainVector);
    vector.push_back(lightVector);
    vector.push_back(light30Vector);

    return vector;
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

            out << (char)digits[i].getLabel();

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
