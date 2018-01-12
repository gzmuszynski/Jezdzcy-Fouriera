#ifndef EXTRACTOR_H
#define EXTRACTOR_H

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
    void featuresExtracted(QImage* image);
    void pixel(int x, int y, QColor* color);

public slots:
    void extractFeatures(QImage* image);

    void sobelEdgeDetection(int x, int y, QImage* img);
    void setSobelPixel(int x, int y, QColor* color);

    void houghEllipseDetection(QImage* img);
    void houghSingle(int x1, int y1, QVector<QPair<int,int>> edges);


private:
    qint16 S[9] = {1, 1, 1,
                   1,-8, 1,
                   1, 1, 1};
    QImage* sobelResult;
    int completed;

    QMutex mutex;
    QMutexLocker *lock;
    QThreadPool *sobelThreadPool;

    int distmin = 10;
    int distmax = 50;

    int accMin = 50;

    double eps = 0.001;

    QVector<ellipse> ellipses;
};

#endif // EXTRACTOR_H
