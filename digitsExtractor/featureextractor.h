#ifndef FEATUREEXTRACTOR_H
#define FEATUREEXTRACTOR_H
#include "digit.h"
#include <QVector>

class FeatureExtractor
{
public:
    static void extract      (QVector<digit> &digits, int task_length);
    static void extractThread(QVector<digit> &digits, int n, int len);
};

#endif // FEATUREEXTRACTOR_H
