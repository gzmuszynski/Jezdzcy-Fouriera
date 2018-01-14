#include "engine.h"
#include "extractor.h"
#include <QImage>
#include <QtMath>
#include <QtDebug>
#include <QFile>

Engine::Engine(QObject *parent) : QObject(parent)
{
    extractor = nullptr;
    radMin = 30;
    radMax = 50;
    increment = 1;
    mapSize = 300;
}

void Engine::loadImage(QString filename)
{
    QImage *image = new QImage(filename);
    before = image;

    if(extractor != nullptr)
        disconnect(this,SIGNAL(requireChange(int,int,int,int)),extractor,SLOT(changeConfiguration(int,int,int,int)));
    extractor = new Extractor(this);
    connect(this,SIGNAL(requireChange(int,int,int,int)),extractor,SLOT(changeConfiguration(int,int,int,int)));

    emit requireChange(radMin, radMax, increment, mapSize);
    emit imageReady(image);
}

void Engine::saveImage(QString filename)
{
    after->save(filename);
}

void Engine::saveData(QString filename)
{
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream ts(&file);

        ts << "@RELATION circles\n\n"
           << "@ATTRIBUTE x\tNUMERIC\n"
           << "@ATTRIBUTE y\tNUMERIC\n"
           << "@ATTRIBUTE r\tNUMERIC\n"
           << "@ATTRIBUTE g\tNUMERIC\n"
           << "@ATTRIBUTE b\tNUMERIC\n"
           << "@ATTRIBUTE class\t{";
        for(unsigned int rgb: classes.keys())
        {
            ts << "\"" << QString::number(rgb,16) << "\"";
            if(rgb == classes.lastKey())
            {
                ts << "}\n\n";
            }
            else
                ts << ",";
        }

        ts << "@DATA\n";
        for(unsigned int rgb: classes.keys())
        {
            for(Element* e:classes[rgb])
            {
                ts << e->getX() << "," << e->getY() << ","  << e->getR()
                   << ","  << e->getG() << ","  << e->getB()<< ",\"" << QString::number(rgb,16) << "\"\n";
            }
        }
    }
    file.close();
}

void Engine::countObjects()
{

    if(!before->isNull())
    {
        if(extractor != nullptr)
            disconnect(this,SIGNAL(requireChange(int,int,int,int)),extractor,SLOT(changeConfiguration(int,int,int,int)));
        extractor = new Extractor(this);
        connect(this,SIGNAL(requireChange(int,int,int,int)),extractor,SLOT(changeConfiguration(int,int,int,int)));

        emit requireChange(radMin, radMax, increment, mapSize);
        connect(extractor,SIGNAL(featuresExtracted(QVector<Element*>)),this,SLOT(showElementsOnImage(QVector<Element*>)));

        extractor->extractFeatures(before);
    }
}

void Engine::showElementsOnImage(QVector<Element*> elements)
{
    after = new QImage(*before);

    int w = after->width();
    int h = after->height();

    int radiusMin = radMin*1.0/600*h;

    int rad2 = radiusMin*radiusMin;

    classes = classifier->createGroupsAndClassify(elements);

    emit classesReady(classes);

    for(unsigned int rgb: classes.keys())
    {
        QVector<Element*> elements = classes[rgb];
        for(Element* element:elements)
        {
//            qDebug() << rgb << element->getX() << element->getY() << element->getR() << element->getG() << element->getB();
            int x = element->getX();
            int y = element->getY();

            for(int x1 = qMax(0,x - radiusMin); x1 < qMin(w,x+radiusMin); x1++)
            {
                for(int y1 = qMax(0,y - radiusMin); y1 < qMin(h,y+radiusMin); y1++)
                {
                    int dx = (x1-x);
                    int dy = (y1-y);
                    int distance = dx*dx+dy*dy;

                    if(distance < rad2)
                    {
                        if((x1 == x && qAbs(dy) < radiusMin*0.75) || (y1 == y && qAbs(dx) < radiusMin*0.75))
                        {
                            after->setPixelColor(x1,y1,QColor(255,255,0));
                        }
                        else if(qMax(qAbs(dx),qAbs(dy)) == (int)(radiusMin*0.2))
                        {
                            after->setPixelColor(x1,y1,QColor(255,255,0));
                        }
                        else
                        {
                            after->setPixel(x1,y1,rgb);
                        }
                    }
                }
            }
        }
    }

    emit imageReady(after);
}

void Engine::changeConfiguration(int radMin, int radMax, int increment, int mapSize)
{
    if(radMin!=0)
        this->radMin = radMin;
    if(radMax!=0)
        this->radMax = radMax;
    if(increment!=0)
        this->increment = increment;
    if(mapSize!=0)
        this->mapSize = mapSize;

    emit requireChange(radMin, radMax, increment, mapSize);
}
