#include "digit.h"

digit::digit(QImage picture) : picture(picture)
{

}

digit::digit(unsigned char label, QImage picture) : digit(picture)
{
    this->label = label;
}

unsigned char digit::getLabel() const
{
    return label;
}

void digit::setLabel(const unsigned char &value)
{
    label = value;
}

QImage digit::getPicture() const
{
    return picture;
}

QVector<float> digit::getFeatures() const
{
    return features;
}

void digit::setFeatures(const QVector<float> &value)
{
    features = value;
}

void digit::addFeature(const float &value)
{

}
