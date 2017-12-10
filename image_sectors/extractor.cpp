#include "extractor.h"
#include <QDir>
#include <QtMath>

Extractor::Extractor()
{

}

void Extractor::extractFeatures(QImage *picture, QString label, int x = 0, int y = 0, int step = 0)
{
    // Average

    double mean = 0;

    for(int x = 0; x < picture->width(); x++)
    {
        for(int y = 0; y < picture->height(); y++)
        {
            QColor col = picture->pixelColor(x,y);
            double pix = col.red();

            mean += col.red();
        }
    }

    mean /= (picture->width()*picture->height());

    for(int x = 0; x < picture->width(); x++)
    {
        for(int y = 0; y < picture->height(); y++)
        {
            QColor col = picture->pixelColor(x,y);
            double s = col.red();
            double rgb = s - mean + 127;
            double n = qBound(0.0, rgb, 255.0);

            picture->setPixelColor(x, y, qRgb(n,n,n));
        }
    }

//    picture->save(QString("D:\\repositories\\builds\\przetwarzanie_obrazu\\sectors\\rand\\%1.bmp").arg(qrand()),"bmp");

    // FFT2D and amplitude spectrum

    QVector<QVector<std::complex<double> > > fft = FFT2D(picture);
    QImage* phase = new QImage(picture->size(), QImage::Format_ARGB32);

    QVector<double> features;

    double side, top;


    for(int k=0;k<fft.size();k++)
    {
        for(int n=0;n<fft.size();n++)
        {
            double real = fft[k][n].real();
            double imag = fft[k][n].imag();

            double rgb = qBound(0.0,qLn(qSqrt((real*real)+(imag*imag)))*32,255.0);
            double pha = qPow(10,qAtan2(real,imag));

            picture->setPixelColor(k, n, qRgb(rgb,rgb,rgb));
            phase->setPixelColor  (k, n, qRgb(pha,pha,pha));

            if((x < 16 || x > 32+16) && (y < 24 || y > 32+24))
                side += rgb;
            if((x < 24 || x > 32+24) && (y < 16 || y > 32+16))
                top += rgb;
        }
    }

    if( side > top )
    {
        QImage* copy = new QImage(*picture);
        for(int x=0;x<fft.size();x++)
        {
            for(int y=0;y<fft.size();y++)
            {
                QColor pix = copy->pixelColor(x,y);

                picture->setPixelColor(y,x,pix);
            }
        }
    }

    double array = 0;
    double cross = 0;
    double dimple = 0;

    double topBar = 0;
    double bottomBar = 0;

    double midBar = 0;

    double crossDif = 0;

    for(int x=0;x<fft.size();x++)
    {
        for(int y=0;y<fft.size();y++)
        {
            double pix = picture->pixelColor(x,y).red();

            if(
               ((x>18 && y >13) && (x<23 && y < 18)) ||
               ((x>41 && y >13) && (x<46 && y < 18)) ||
               ((x>18 && y >47) && (x<23 && y < 52)) ||
               ((x>41 && y >47) && (x<46 && y < 52))
              )
            {
                array += pix;
            }

            if(
               ((x>21 && y >23) && (x<28 && y < 30)) ||
               ((x>39 && y >29) && (x<46 && y < 36))
              )
            {
                dimple += pix;
            }

            if((y>14 && y < 17))
            {
                topBar += pix;
            }

            if((y>48 && y < 51))
            {
                bottomBar += pix;
            }

            if(
               ((x>30 && y >30) && (x<35 && y < 35)) &&
               !(x==32 && y==32)
              )
            {
                cross+=pix;
            }

            if(x>29 && x <36)
            {
                midBar+=pix;
            }

            if(((x>25 && y >30) && (x<40 && y < 35)))
            {
                crossDif += pix;
            }
            if(((x>30 && y >25) && (x<35 && y < 40)))
            {
                crossDif -= pix;
            }
        }
    }


    features.push_back(array);
    features.push_back(dimple);

    features.push_back(cross);
    features.push_back(crossDif);

    features.push_back(topBar);
    features.push_back(bottomBar);

    features.push_back(midBar);

    Element* element = new Element(label, features);

    element->setFft(picture);
    element->setPhase(phase);

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
