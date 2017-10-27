#ifndef DIGIT_H
#define DIGIT_H

#include <QImage>
#include <qstring.h>

// class representing a MISNT digit

class digit
{
public:
    digit()                                                       { }
    digit(QImage picture)                      : picture(picture) { }
    digit(unsigned char label, QImage picture) : digit(picture)   { this->label = label; }
    digit(unsigned char label, QImage picture, QVector<float> features)
                                               : digit(label, picture)
                                                                  { this->features = features; }

    QImage         getPicture () const                            { return picture; }

    unsigned char  getLabel   () const                            { return label; }
    void           setLabel   (const unsigned char &value)        { label = value; }

    QVector<float> getFeatures() const                            { return features; }
    void           setFeatures(const QVector<float> &value)       { features = value; }
    void           addFeature (const float &value)                { features.push_back(value); }

private:
    unsigned char  label;
    QImage         picture;
    QVector<float> features;
};

#endif // DIGIT_H
