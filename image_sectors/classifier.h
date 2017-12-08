#ifndef CLASSIFIER_H
#define CLASSIFIER_H
#include <class.h>
#include <QObject>
#include <QMap>

class Classifier : public QObject
{
    Q_OBJECT
public:
    Classifier();

    void setMetric(int value);

public slots:
    virtual void process(Element* element, int x, int y, int step) = 0;
    virtual void setClassElements(QMap<QString,Class*> classes, QVector<Element*> elements) = 0;

signals:
    void classified(QString className, int x, int y, int step);

protected:

    QMap<QString,Class*> classes;
    QVector<Element*> elements;

    void normalizeClasses();
    void normalize(Element* element);
    int metric = 2;
    double distance(Element* from, Element* to);

    QVector<double> mins;
    QVector<double> maxes;
};

#endif // CLASSIFIER_H
