#ifndef DIGIT_H
#define DIGIT_H

#include <QImage>
#include <qstring.h>

// class representing a MISNT digit

class Element
{
public:
    Element()                                                       { }
    Element(QImage picture)                      : picture(picture) { }
    Element(unsigned char label, QImage picture) : Element(picture)   { this->label = label; }
    Element(unsigned char label, QVector<float> features)           { this->label = label; this->features = features; }

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
