#include "extractor.h"
#include <QDir>
#include <QtMath>

Extractor::Extractor()
{

}

void Extractor::extractFeatures(QImage *picture, QString label, int x = 0, int y = 0, int step = 0)
{
    QVector<QVector<std::complex<double> > > fft = FFT2D(picture);
    QVector<double> features;

    for(int k=0;k<fft.size();k++)
    {
        for(int n=0;n<fft.size();n++)
        {
            double real = fft[k][n].real();
            double imag = fft[k][n].imag();
            double rgb = qBound(0.0,qLn(qSqrt((real*real)+(imag*imag)))*32,255.0);
            picture->setPixelColor(k,n,qRgb(rgb,rgb,rgb));
            if(k == 0)
                features.push_back(rgb);
        }
    }
    Element* element = new Element(label, features);
    element->setFft(picture);

    emit featuresExtracted(element, x, y, step);
}


QVector<std::complex<double>> Extractor::FFT1D(QVector<std::complex<double>> input, int N)
{
    int n = input.size();
    if(n==1)
        return input;

    std::complex<double> W=1;
    std::complex<double> W_n = std::exp(std::complex<double>(0.0,1.0)*(2*M_PI/n));
    QVector<std::complex<double>> Y0 = FFT1D(even(input),n);
    QVector<std::complex<double>> Y1 = FFT1D(odd(input),n);
    QVector<std::complex<double>> Y0_k;
    QVector<std::complex<double>> Y1_k;
    for(int k=0;k<std::min(Y0.size(),Y1.size());k++){
        Y0_k.push_back(Y0[k]+Y1[k]*W);
        Y1_k.push_back(Y0[k]-Y1[k]*W);
        if(n==N /*&& !reverse*/){
            Y0_k[k]*=1.0/qSqrt(n);
            Y1_k[k]*=1.0/qSqrt(n);
        }
        W=W*W_n;
        std::complex<double> W_n = std::exp(std::complex<double>(0.0,1.0)*(2*M_PI*k/n));
    }
    return Y0_k+Y1_k;
}

QVector<std::complex<double> > Extractor::even(QVector<std::complex<double>> vec)
{
    QVector<std::complex<double>> result;
    for(int i=0;i<vec.size();i+=2)
        result.push_back(vec[i]);
    return result;
}

QVector<std::complex<double>> Extractor::odd(QVector<std::complex<double>> vec)
{
    QVector<std::complex<double>> result;
    for(int i=1;i<vec.size();i+=2)
        result.push_back(vec[i]);
    return result;
}
QVector<QVector<std::complex<double> > > Extractor::FFT2D(QImage *picture)
{
    int h = picture->height();
    int w = picture->width();

    int size2 = qNextPowerOfTwo(qMin(h,w)-1); // 2D FFT works on power of 2 size squares

    // row-column algorithm 2D_FFT = 1D_FFT by columns (1D_FFT by rows (image) )

    // row section


    QVector<QVector<std::complex<double>>> rows, columns;
    for(int x = 0; x < size2; x++)
    {
        QVector<std::complex<double>> row;
        for(int y = 0; y < size2; y++)
        {
            if(x < w && y < h)
                row.push_back(std::complex<double>(qGray(picture->pixel(x,y)), 0.0));
            else
                row.push_back(std::complex<double>(0.0, 0.0));
        }

        rows.push_back(FFT1D(row, size2));
    }

    // column section

    for(int x = 0; x < size2; x++)
    {
        QVector<std::complex<double>> column;
        for(int y = 0; y < size2; y++)
        {
            column.push_back(rows[y][x]);
        }

        columns.push_back(FFT1D(column, size2));
    }

    // shift by half size

    QVector<QVector<std::complex<double>>> final(columns);

    for(int x = 0; x < size2; x++)
    {
        for(int y = 0; y < size2; y++)
        {
            final[x][y] = columns[(int)(x+size2*0.5)%size2][(int)(y+size2*0.5)%size2];
        }
    }

    return final;
}
