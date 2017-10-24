#ifndef DIGIT_H
#define DIGIT_H

#include <QImage>
#include <qstring.h>



class digit
{
public:
    digit();
private:
    QString label;
    QImage picture;
    QVector<float> features;
};

#endif // DIGIT_H
