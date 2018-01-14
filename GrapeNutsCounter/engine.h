#ifndef ENGINE_H
#define ENGINE_H

#include "classifier.h"
#include "element.h"

#include <QMap>
#include <QObject>
#include <QVector>

class Extractor;

class Engine : public QObject
{
    Q_OBJECT
public:
    explicit Engine(QObject *parent = nullptr);

signals:
    void objectsCounted(int count);
    void imageReady(QImage* image);

    void requireChange(int radMin, int radMax, int increment, int mapSize);
    void classesReady(QMap<uint,QVector<Element*> > classes);

public slots:
    void loadImage(QString filename);
    void saveImage(QString filename);
    void saveData(QString filename);

    void changeConfiguration(int radMin, int radMax, int increment,int mapSize);

    void countObjects();
    void showElementsOnImage(QVector<Element *> elements);

private:
    Extractor* extractor;
    QImage* before;
    QImage* after;

    Classifier* classifier;
    QVector<Element*> elements;
    QMap<unsigned int,QVector<Element*>> classes;

    int radMin, radMax, increment, mapSize;
};

#endif // ENGINE_H
