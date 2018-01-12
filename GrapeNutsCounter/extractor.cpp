#include "extractor.h"
#include <QImage>
#include <QtConcurrent/QtConcurrent>
#include <QColor>
#include <QDebug>
#include <QVector>


Extractor::Extractor(QObject *parent) : QObject(parent)
{
    lock = new QMutexLocker(&mutex);
    sobelThreadPool = new QThreadPool(this);
}

void Extractor::extractFeatures(QImage *image)
{

//    image = new QImage(image->scaledToHeight(256));

    sobelResult = new QImage(image->size(), image->format());

    connect(this,SIGNAL(pixel(int,int,QColor*)),this,SLOT(setSobelPixel(int,int,QColor*)));
//    connect(this,SIGNAL(featuresExtracted(QImage*)),this,SLOT(houghEllipseDetection(QImage*)));

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
        emit featuresExtracted(sobelResult);
    }
    lock->unlock();
}

void Extractor::houghEllipseDetection(QImage *img)
{
    int w = img->width();
    int h = img->height();

}

void Extractor::houghSingle(int x1, int y1, QVector<QPair<int, int> > edges)
{
}
