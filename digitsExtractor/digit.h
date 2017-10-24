#ifndef DIGIT_H
#define DIGIT_H

#include <QImage>
#include <qstring.h>

// class representing a MISNT digit

class digit
{
public:
    digit() {}
    digit(QImage picture);
    digit(unsigned char label, QImage picture);

    QImage getPicture() const;

    unsigned char getLabel() const;
    void    setLabel(const unsigned char &value);

    QVector<float> getFeatures() const;
    void           setFeatures(const QVector<float> &value);
    void           addFeature (const float &value);

private:
    unsigned char label;
    QImage picture;
    QVector<float> features;
};

#endif // DIGIT_H
