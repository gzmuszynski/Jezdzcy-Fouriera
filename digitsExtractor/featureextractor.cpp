#include "featureextractor.h"
#include <QtConcurrent/QtConcurrent>

void FeatureExtractor::extract(QVector<digit> &digits, int task_length)
{
    qDebug() << "--------Feature Extractor--------";
    qDebug() << "Separating task into threads";

    for(int n = 0; n < digits.size(); n += task_length)
    {
        QtConcurrent::run(extractThread, std::ref(digits), n, task_length);
    }

    qDebug() << "Wait for tasks";
    QThreadPool::globalInstance()->waitForDone();

    qDebug() << "Step finished\n";
}

void FeatureExtractor::extractThread(QVector<digit> &digits, int n, int len)
{

    for(int i = n; i < n+len; i++)
    {
        QImage picture = digits[i].getPicture();

        float sum = 0.0f;

        float sumT = 0.0f;
        float sumB = 0.0f;
        float sumL = 0.0f;
        float sumR = 0.0f;

        float crossT =  0.0f;
        float crossB =  0.0f;
        float crossL =  0.0f;
        float crossR =  0.0f;

        for(int x = 0; x < picture.width(); x++)
        {
            for(int y = 0; y < picture.height(); y++)
            {
                float current = 1 - picture.pixelColor(x,y).valueF();
                sum += current;

                if(y > picture.height() * 0.5)
                {
                    sumT += current;
                    if(x > picture.width() * 0.4 && x < picture.width() * 0.6)
                    {
                        crossT += current;
                    }

                }
                else
                {
                    sumB += current;
                    if(x > picture.width() * 0.4 && x < picture.width() * 0.6)
                    {
                        crossB += current;
                    }
                }

                if(x > picture.width() * 0.5)
                {
                    sumR += current;
                    if(y > picture.height() * 0.4 && y < picture.height() * 0.6)
                    {
                        crossR += current;
                    }
                }
                else
                {
                    sumL += current;
                    if(y > picture.height() * 0.4 && y < picture.height() * 0.6)
                    {
                        crossL += current;
                    }
                }
            }
        }

        digits[i].addFeature( sum );

        digits[i].addFeature( sumT );
        digits[i].addFeature( sumB );
        digits[i].addFeature( sumL );
        digits[i].addFeature( sumR );

        digits[i].addFeature( crossT );
        digits[i].addFeature( crossB );
        digits[i].addFeature( crossL );
        digits[i].addFeature( crossR );
    }
}
