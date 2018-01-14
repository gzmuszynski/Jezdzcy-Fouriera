#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include "element.h"

#include <QObject>
#include <QMutexLocker>
#include <QColor>
#include <QThreadPool>
#include <QVector>

struct ellipse{
  double x0,y0,a,b,alfa;
};

class Extractor : public QObject
{
    Q_OBJECT
public:
    explicit Extractor(QObject *parent = nullptr);

signals:
    void featuresExtracted(QVector<Element*> elements);
    void edgesDetected(QImage* image);
    void pixel(int x, int y, QColor* color);

public slots:
    void extractFeatures(QImage* image);

    void sobelEdgeDetection(int x, int y, QImage* img);
    void setSobelPixel(int x, int y, QColor* color);

    void houghCircleDetection(QImage* img);
    void houghSingleRadius(int r, QImage *image);

    void changeConfiguration(int radMin, int radMax, int increment, int mapSize);
    void groupPixels(int radius, QImage* image);
    void createElements(QImage* circleMap);
private:
    qint16 S[9] = {1, 1, 1,
                   1,-8, 1,
                   1, 1, 1};
    QImage* sobelResult;
    QImage* originalCopy;

    int completed;

    QMutex mutex;
    QMutexLocker *lock;
    QThreadPool *sobelThreadPool;

    QVector<QImage*> radiusMaps;
    QVector<QVector<int>>* accumulator;

    double min;
    double max;
    int increment;
    int mapSize;


};

#endif // EXTRACTOR_H
