#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include "element.h"
#include <QImage>
#include <QObject>
#include "class.h"


class Extractor : public QObject
{
public:
    Q_OBJECT
public:
    Extractor();

    void extractFeatures(QImage* picture, QString label, int x, int y, int step);

signals:
    void featuresExtracted(Element* element, int x, int y, int step);

private:
    QVector<std::complex<double> >           FFT1D(QVector<std::complex<double> > input, int N);
    QVector<QVector<std::complex<double> > > FFT2D(QImage* picture);
    QVector<std::complex<double> > even(QVector<std::complex<double> > vec);
    QVector<std::complex<double> > odd(QVector<std::complex<double> > vec);

    double windowFunc(double n, int N);
};

#endif // EXTRACTOR_H
