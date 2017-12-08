#include "element.h"

Element::Element(QString label, QVector<double > features) : label(label), features(features)
{

}

QString Element::getLabel() const
{
    return label;
}

QVector<double> Element::getFeatures() const
{
    return features;
}

QImage *Element::getFft() const
{
    return fft;
}

void Element::setFft(QImage *value)
{
    fft = value;
}
