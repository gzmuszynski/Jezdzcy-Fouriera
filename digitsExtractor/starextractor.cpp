#include "starextractor.h"
#include <QtDebug>
#include <QtConcurrent>
#include <QtMath>


void STARExtractor::extract(QVector<QVector<Element> > &elements)
{
    qDebug() << "--------Feature Extractor--------";
    qDebug() << "Separating task into threads";

    for(int v = 0; v < elements.size(); v++)
    {
        for(int n = 0; n < elements[v].size(); n++)
        {
            QtConcurrent::run(extractThread, std::ref(elements[v][n]));
        }
    }
    qDebug() << "Wait for tasks";
    QThreadPool::globalInstance()->waitForDone();

    qDebug() << "Step finished\n";
}

void STARExtractor::extractThread(Element &element)
{
    QImage picture = element.getPicture();
    QImage median(picture);

    float average = 0.0f;

    float sum = 0.0f;

    double sum0  = 0.0f;
    double sum30 = 0.0f;
    double sum70 = 0.0f;

    float count0  = 0.0f;
    float count30 = 0.0f;
    float count70 = 0.0f;

    float pivotX = 0.0f;
    float pivotY = 0.0f;

    int width = picture.width();
    int height= picture.height();

    float x1 = 0.0f; float y1 = 0.0f;
    float x2 = 0.0f; float y2 = 0.0f;

    float x3 = 0.0f; float y3 = 0.0f;
    float x4 = 0.0f; float y4 = 0.0f;

    int minx = width; int miny = height;
    int maxx = 0.0f;  int maxy = 0.0f;

    unsigned char min = 255;

    bool first70 = true;
    bool first30 = true;

    float intersections = 0.0f;
    bool reset = false;

    float center = 0.0f;

    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            QVector<unsigned char> vec;
            for(int i = 0; i < 9; i++)
            {
                int xi = x+i%3-1;
                int yi = y+i/3-1;
                if(xi < width && xi >= 0 && yi < height && yi >= 0)
                {
                    unsigned char pixel = picture.pixelColor(xi,yi).green();
                    vec.push_back(pixel);
                }
            }
            qSort(vec);
            unsigned char med = vec[(int)(vec.size()*0.5)];
            median.setPixelColor(x,y,qRgb(med,med,med));

            unsigned char pixel = picture.pixelColor(x,y).green();
            min = qMin(min, pixel);

            average += pixel;
        }
    }
    average /= picture.height() * picture.width();

    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            unsigned char med = average - median.pixelColor(x,y).green();
            if(med > 45)
            {
                minx = qMin(minx,x);
                miny = qMin(miny,y);
                maxx = qMax(maxx,x);
                maxy = qMax(maxy,y);
            }
            median.setPixelColor(x,y,qRgb(med,med,med));
        }
    }
    maxx -= (minx-5);
    maxy -= (miny-5);
    minx -= 5;
    miny -= 5;
    picture = picture.copy(minx,miny,maxx,maxy).scaledToHeight(256,Qt::FastTransformation);
    median  = median. copy(minx,miny,maxx,maxy).scaledToHeight(256,Qt::FastTransformation);

    for(int x = 0; x < picture.width(); x++)
    {
        for(int y = 0; y < picture.height(); y++)
        {
            unsigned char pixel = picture.pixelColor(x,y).green();
            unsigned char medel = median. pixelColor(x,y).green();

            float div = 1.0/256;
            float weight = pixel*div;
            sum += qBound(0,pixel-medel,255);

            pivotX += weight * (x-127);
            pivotY += weight * (y-127);

            int xx = x-128;
            int yy = y-128;

            if(pixel < average - 70)
            {
                if((xx*xx)+(yy*yy) < 264)
                {
                    center += pixel;
                }

                if(first70)
                {
                    x1 = x;
                    y1 = y;
                    first70 = false;
                }
                x2 = x;
                y2 = y;

                sum70 += pixel;
                count70++;
            }
            else if(pixel < average - 40)
            {
                if(first30)
                {
                    x3 = x;
                    y3 = y;
                    first30 = false;
                }
                x3 = x;
                y3 = y;

                sum30 += pixel;
                count30++;
            }
            else if(pixel < average - 20)
            {
                sum0 += pixel;
                count0++;
            }
        }
    }

    if(sum70 != 0.0f) sum70 /= count70;
    if(sum30 != 0.0f) sum30 /= count30;
    if(sum0  != 0.0f) sum0  /= count0;

    x2 -= x1;
    y2 -= y1;
    float distance70 = qSqrt((x2*x2)+(y2*y2));
    x4 -= x3;
    y4 -= y3;
    float distance30 = qSqrt((x4*x4)+(y4*y4));
    float distancepiv = qSqrt((pivotX*pivotX)+(pivotY*pivotY));

    element.addFeature(sum);

    element.addFeature(sum70);
    element.addFeature(sum30);
    element.addFeature(sum0);

    element.addFeature(count70);
    element.addFeature(count30);
    element.addFeature(count0);

//    element.addFeature(distance70);
    element.addFeature(distancepiv);
//        element.addFeature(distance30);

    element.addFeature(min);
}
