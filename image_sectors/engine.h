#ifndef ENGINE_H
#define ENGINE_H
#include <QObject>
#include "classifier.h"
#include "extractor.h"
#include <QMap>
#include <QMutex>
#include <QThreadPool>
#include <QTime>



class Engine : public QObject
{
public:
    Engine();

    Q_OBJECT
public:
    void loadImage(QString filename, QString labels);
    void train(QString directory);
    void classify();

    void saveFFT(QString directory);
    void saveClasses(QString filename);
    void openClasses(QString filename);
    void saveImage(QString filename);

    void showError();
    void showLabel();
    void showInput();

    mutable QMutex lock;
public slots:
    void changeClassifier(int newClassifier);
    void changeMetric(int newMetric);
    void changeK(int near);

    void addElementToClass(Element* element);
    void finalizeClassification(QString className, int x, int y, int step);
    void adaptiveClassifier(int step);

signals:
    void engineBusy(bool isTrue);
    void engineTrained(int classesNum);
    void imageReady(QImage* image);
    void classifierDone(int recall);
    void classesReady(QMap<QString,Class*> classes, QVector<Element*> elements);

    void trainProgress(int trained, int overall);
    void stepFinished(int step);


private:
    QVector<Classifier*> classifiers;
    Extractor*           extractor;

    QMap<QString, Class*> classes;
    QVector<Element*>     elements;

    int currentClassifier;
    QThreadPool classifierThreadPool;

    QImage* labels;
    QImage* input;
    QImage* inputLabels;


    int trainItemCount;
    int trainedItemCount;

    int leftToCompute;
    int step;

    QTime stepBegin;
};

#endif // ENGINE_H
