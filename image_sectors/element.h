#ifndef ELEMENT_H
#define ELEMENT_H
#include <QImage>
#include <QVector>
#include <complex>


class Element
{
public:
    Element(QString label, QVector<double> features);

    QString getLabel() const;
    QVector<double> getFeatures() const;

    QImage *getFft() const;
    void setFft(QImage *value);

    QImage *getPhase() const;
    void setPhase(QImage *value);

    void setFeatures(const QVector<double> &value);

private:
    QString label;
    QVector<double> features;
    QImage* fft;
    QImage* phase;
};

#endif // TEXTURE_H
