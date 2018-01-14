#include "element.h"
#include "extractor.h"
#include <QImage>
#include <QtConcurrent/QtConcurrent>
#include <QColor>
//#include <QDebug>
#include <QVector>


Extractor::Extractor(QObject *parent) : QObject(parent)
{
    lock = new QMutexLocker(&mutex);
    sobelThreadPool = new QThreadPool(this);

//    min = 20;
//    max = 70;
//    increment = 1;
//    mapSize = 600;
}

void Extractor::extractFeatures(QImage *image)
{

    originalCopy = new QImage(*image);
    image = new QImage(image->scaledToHeight(mapSize));

    sobelResult = new QImage(image->size(), image->format());

    connect(this,SIGNAL(pixel(int,int,QColor*)),this,SLOT(setSobelPixel(int,int,QColor*)));
    connect(this,SIGNAL(edgesDetected(QImage*)),this,SLOT(houghCircleDetection(QImage*)));

    sobelThreadPool->clear();
    lock->unlock();

    completed = 0;
    for(int y = 0; y < image->height(); y++)
    {
        QtConcurrent::run(this,&Extractor::sobelEdgeDetection,0,y,image);
    }
}

void Extractor::sobelEdgeDetection(int x, int y, QImage *img)
{
    int w = img->width();
    int h = img->height();

    QColor* line = new QColor[w];

    for(int x = 0; x < w; x++)
    {
        qint16 r = 0;
        qint16 g = 0;
        qint16 b = 0;

        for(int s = 0; s < 3; s++)
        {
            for(int t = 0; t < 3; t++)
            {
                int st = s+3*t;
                int xs = x+s-1;
                int yt = y+t-1;

                if(xs>=0 && yt >=0 && xs < w && yt < h)
                {
                    QColor pixel = img->pixelColor(xs,yt);

                    r += S[st]*pixel.red()  ;
                    g += S[st]*pixel.green();
                    b += S[st]*pixel.blue() ;
                }
            }
        }

        QColor final(qBound((qint16)0,r,(qint16)255),
                     qBound((qint16)0,g,(qint16)255),
                     qBound((qint16)0,b,(qint16)255));
        line[x] = final;
    }
    emit pixel(w,y,line);

}

void Extractor::setSobelPixel(int x, int y, QColor* color)
{
    lock->relock();

    completed++;
    for(int i = 0; i < x; i++)
    {
        sobelResult->setPixelColor(i,y,color[i]);
    }

    if(completed == sobelResult->height())
    {
        emit edgesDetected(sobelResult);
    }
    lock->unlock();
}

void Extractor::houghCircleDetection(QImage *img)
{
    int w = img->width();
    int h = img->height();

    int radiusMin = min/600*mapSize;
    int radiusMax = max/600*mapSize;

    for(int R = radiusMin; R < radiusMax; R+=increment)
    {
        QtConcurrent::run(this,&Extractor::houghSingleRadius,R,img);
    }
    QThreadPool::globalInstance()->waitForDone();


    QVector<QVector<int>> accumulator(w,QVector<int>(h, 0.0));
    for(QImage* image : radiusMaps)
    {
        for(int x = 0; x < w; x++)
        {
            for(int y = 0; y < h; y++)
            {
                int pixel = qGray(image->pixelColor(x,y).rgb());
                accumulator[x][y]+=pixel;
            }
        }
    }

    QImage* output = new QImage(w,h,QImage::Format_ARGB32);

    for(int x = 0; x < w; x++)
    {
        for(int y = 0; y < h; y++)
        {
            int pixel = qBound(0,accumulator[x][y],255);
            output->setPixelColor(x,y,QColor(pixel,pixel,pixel));
        }
    }

    groupPixels(radiusMin+(radiusMax-radiusMin)/2,output);
}

void Extractor::houghSingleRadius(int r, QImage* image)
{
    int w = image->width();
    int h = image->height();

    QVector<QVector<int>> accumulator(w,QVector<int>(h, 0.0));

    for(int x = 0; x < w; x++)
    {
        for(int y = 0; y < h; y++)
        {
            qint16 pixel = qGray(image->pixelColor(x,y).rgb());

            if(pixel > 50)
            {
                for(int theta = 0; theta < 360; theta++)
                {
                    int a = x - r * qCos(theta * M_PI / 180);
                    int b = y - r * qSin(theta * M_PI / 180);
                    if(a >= 0 && a < w && b >= 0 && b < h)
                        accumulator[a][b]++;
                }
            }
        }
    }
    QImage* radiusMap = new QImage(w,h,QImage::Format_ARGB32);
    for(int x = 0; x < w; x++)
    {
        for(int y = 0; y < h; y++)
        {
            int pixel = qBound(0,accumulator[x][y],255);
            if(pixel > 100)
            {
                radiusMap->setPixelColor(x,y,QColor(255,255,255));
            }
            else
            {
                radiusMap->setPixelColor(x,y,QColor(0,0,0));
            }

        }
    }
    lock->relock();
    radiusMaps.push_back(radiusMap);
    lock->unlock();
}

void Extractor::changeConfiguration(int radMin, int radMax, int increment, int mapSize)
{
    if(radMin!=0)
        min = radMin;
    if(radMax!=0)
        max = radMax;
    if(increment!=0)
        this->increment = increment;
    if(mapSize!=0)
        this->mapSize = mapSize;
}

void Extractor::groupPixels(int radius, QImage *image)
{
    int rad2 = radius*radius;
    for(int x = 0; x < image->width(); x++)
    {
        for(int y = 0; y < image->height(); y++)
        {
            int pixel = qGray(image->pixelColor(x,y).rgb());

            if(pixel > 127)
            {
                image->setPixelColor(x,y,QColor(0,0,0));

                int x0 = x;
                int y0 = y;
                int count = 1;

                for(int x1 = qMax(0,x - radius); x1 < qMin(image->width(),x+radius); x1++)
                {
                    for(int y1 = qMax(0,y - radius); y1 < qMin(image->height(),y+radius); y1++)
                    {
                        int dx = (x1-x);
                        int dy = (y1-y);
                        int distance = dx*dx+dy*dy;

                        if(distance < rad2)
                        {
                            int pixel = qGray(image->pixelColor(x1,y1).rgb());
                            if(pixel > 127)
                            {
                                image->setPixelColor(x1,y1,QColor(0,0,0));

                                x0+=x1;
                                y0+=y1;
                                count++;
                            }
                        }
                    }
                }
                x0 /= count;
                y0 /= count;

                image->setPixelColor(x0,y0,QColor(255,255,255));
            }
        }
    }
    createElements(image);
}

void Extractor::createElements(QImage *circleMap)
{
    QVector<Element*> elements;

    int w = originalCopy->width();
    int h = originalCopy->height();

    int radius = min/600*h;
    int rad2 = radius*radius;


    for(int x = 0; x < circleMap->width(); x++)
    {
        for(int y = 0; y < circleMap->height(); y++)
        {
            int pixel = qGray(circleMap->pixelColor(x,y).rgb());

            if(pixel > 127)
            {
                int x0 = ((double)x)/circleMap->width()*w;
                int y0 = ((double)y)/circleMap->height()*h;

                int r = 0, g = 0, b = 0, count = 0;

                for(int x1 = qMax(0,x0-radius); x1 < qMin(w, x0+radius); x1++)
                {
                    for(int y1 = qMax(0,y0-radius); y1 < qMin(h, y0+radius); y1++)
                    {
                        int dx = x1-x0;
                        int dy = y1-y0;

                        if(dx*dx+dy*dy < rad2)
                        {
                            QColor pixel = originalCopy->pixelColor(x1,y1).rgb();

                            r += pixel.red();
                            g += pixel.green();
                            b += pixel.blue();
                            count++;
                        }
                    }
                }
                r /= count;
                g /= count;
                b /= count;

                if(QColor(r,g,b).saturation() > 30)
                    elements.push_back(new Element(x0,y0,r,g,b));
            }
        }
    }
    emit featuresExtracted(elements);
}
