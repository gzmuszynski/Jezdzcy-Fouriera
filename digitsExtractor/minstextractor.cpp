#include "featureextractor.h"
#include <QtConcurrent/QtConcurrent>

#define DEBUG false

void FeatureExtractor::extract(QVector<Element> &digits, int task_length)
{
    qDebug() << "--------Feature Extractor--------";
    qDebug() << "Separating task into threads";

    if( DEBUG )
    {
        digits = digits.mid(0,1000);
    }

    for(int n = 0; n < digits.size(); n += task_length)
    {
        int len = n + task_length >= digits.size()?  digits.size()-n : task_length;
        QtConcurrent::run(extractThread, std::ref(digits), n, len);
    }

    qDebug() << "Wait for tasks";
    QThreadPool::globalInstance()->waitForDone();

    qDebug() << "Step finished\n";
}

void FeatureExtractor::extractThread(QVector<Element> &digits, int n, int len)
{

    bool reset = false;

    for(int i = n; i < n+len; i++)
    {
        QImage picture = digits[i].getPicture();

        float sum = 0.0f;

        float sumT = 0.0f;
        float sumB = 0.0f;
        float sumL = 0.0f;
        float sumR = 0.0f;

        float pivotX = 0.0f;
        float pivotY = 0.0f;

        float horizontal = 0.0f;
        float vertical = 0.0f;

        float midRow =  0.0f;
        float quarterRow =  0.0f;
        float thirdRow = 0.0f;

        float midIntersections =  0.0f;
        float quarterIntersections =  0.0f;
        float thirdIntersections = 0.0f;
        bool  midReset = false;
        bool  quarterReset = false;
        bool  thirdReset = false;

        float diag =  0.0f;
        float backDiag =  0.0f;

        float circleIntersections = 0.0f;

        for(int x = 0; x < picture.width(); x++)
        {
            for(int y = 0; y < picture.height(); y++)
            {
                float current = 1 - picture.pixelColor(x,y).valueF();
                float current2 = 1 - picture.pixelColor(x, picture.height()-y-1).valueF();
                float current3 = 1 - picture.pixelColor(picture.width()-x-1, y).valueF();

                sum += current;

                if(y > picture.height() * 0.5)
                    sumT += current;
                else
                    sumB += current;

                if(x > picture.width() * 0.5)
                    sumR += current;
                else
                    sumL += current;

                if(x == y)
                    diag += current;
                if(x == picture.height()-y)
                    backDiag += current;

                if(current > 0.5f)
                {
                    pivotX += x - (picture.width() * 0.5f);
                    pivotY += y - (picture.height() * 0.5f);
                }

                if(y == (int)(picture.height() * 0.5))
                {
                    if(current > 0.5f)
                    {
                        if(midRow == 0)
                        {
                            midRow = x;
                        }
                        if(!midReset)
                        {
                            midIntersections++;
                            midReset = true;
                        }
                    }
                    else
                    {
                        midReset = false;
                    }
                }

                if(y == (int)(picture.height() * 0.35))
                {
                    if(current > 0.5f)
                    {
                        if(quarterRow == 0)
                        {
                            quarterRow = x;
                        }
                        if(!quarterReset)
                        {
                            quarterIntersections++;
                            quarterReset = true;
                        }
                    }
                    else
                    {
                        quarterReset = false;
                    }

                }

                if(y == (int)(picture.height() * 0.65))
                {
                    if(current > 0.5f)
                    {
                        if(thirdRow == 0)
                        {
                            thirdRow = x;
                        }
                        if(!thirdReset)
                        {
                            thirdIntersections++;
                            thirdReset = true;
                        }
                    }
                    else
                    {
                        thirdReset = false;
                    }

                }

                int x2 = x - picture.width() * 0.5f;
                int y2 = y - picture.height() * 0.5f;

                if( (x2*x2) + (y2*y2) > (int)(picture.width() * 0.4f) && (x2*x2) + (y2*y2) < (int)(picture.width() * 0.6f))
                {
                    if(current > 0.5f)
                    {
                        if(reset)
                        {
                            circleIntersections++;
                            reset = false;
                        }
                    }
                    else
                        reset = true;
                }

                if(current == current2)
                {
                    vertical++;
                }
                if(current == current3)
                {
                    horizontal++;
                }
            }
        }

        digits[i].addFeature( sum );

        digits[i].addFeature( sumT );
        digits[i].addFeature( sumB );
        digits[i].addFeature( sumL );
        digits[i].addFeature( sumR );

//        digits[i].addFeature( pivotX );
//        digits[i].addFeature( pivotY );

        digits[i].addFeature( horizontal );
        digits[i].addFeature( vertical );

        digits[i].addFeature( circleIntersections );

        digits[i].addFeature( midRow );
        digits[i].addFeature( quarterRow );
        digits[i].addFeature( thirdRow );

        digits[i].addFeature( midIntersections );
        digits[i].addFeature( quarterIntersections );
        digits[i].addFeature( thirdIntersections );

//        digits[i].addFeature( diag );
//        digits[i].addFeature( backDiag );
    }
}
