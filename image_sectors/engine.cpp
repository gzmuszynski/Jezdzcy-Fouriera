#include "engine.h"
#include <QDir>
#include <QImage>
#include <QtConcurrent/QtConcurrent>
#include "knearestneighbourclassifier.h"
#include "patternclassifier.h"
#include <QtDebug>

Engine::Engine()
{
    extractor = new Extractor();
    classifiers.push_back(new KNearestNeighbourClassifier());
    classifiers.push_back(new PatternClassifier());

    input       = nullptr;
    labels      = nullptr;
    inputLabels = nullptr;

    currentClassifier = 0;
}

void Engine::loadImage(QString filename, QString labels)
{
    input = new QImage(QImage(filename).convertToFormat(QImage::Format_ARGB32));
    inputLabels = new QImage(labels);
    if(!input->isNull())
        emit imageReady(std::ref(input));
}

void Engine::train(QString directory)
{
    emit engineBusy(true);
    KNearestNeighbourClassifier* knn = (KNearestNeighbourClassifier*)classifiers[0];
    PatternClassifier* pc = (PatternClassifier*)classifiers[1];
    classes.clear();
    disconnect(SIGNAL(featuresExtracted(Element*, int, int, int)));
    connect(extractor,SIGNAL(featuresExtracted(Element*, int, int, int)),this,SLOT(addElementToClass(Element*)));
    connect(this,SIGNAL(classesReady (QMap<QString,Class*>, QVector<Element*>)),
            knn,SLOT(setClassElements(QMap<QString,Class*>, QVector<Element*>)));
    connect(this,SIGNAL(classesReady (QMap<QString,Class*>, QVector<Element*>)),
            pc,SLOT(setClassElements (QMap<QString,Class*>, QVector<Element*>)));


    QDir root(directory);

    QStringList dirList = root.entryList(QDir::Dirs);
    dirList.removeFirst();
    dirList.removeFirst();
    QMap<QString, QString> itemList;

    trainedItemCount = 0;
    trainItemCount   = 0;

    for(QString dirName:dirList)
    {
        QDir dir(root.absoluteFilePath(dirName));
        QStringList dirItemList = dir.entryList(QDir::Files);

        for(QString item:dirItemList)
        {
            if(item > 0 && itemList.size() < 100)
            {
                itemList[dir.absoluteFilePath(item)] = dirName;
            }
        }
    }


    trainItemCount = itemList.size();
    for(QString item:itemList.keys())
    {
        QImage* image = new QImage(QImage(item).convertToFormat(QImage::Format_ARGB32));;
//        qDebug() << item;
        QtConcurrent::run(QThreadPool::globalInstance(),extractor,&Extractor::extractFeatures,image, itemList[item], 0, 0, 0);
    }
}

void Engine::classify()
{
    if(input != nullptr && !(classes.isEmpty()))
    {
        emit engineBusy(true);

        QThreadPool classifierThreadPool;
        Classifier* classifier = classifiers[currentClassifier];

//        classifierThreadPool.setMaxThreadCount(2);

        disconnect(SIGNAL(featuresExtracted(Element*, int, int, int)));
        connect(extractor,SIGNAL(featuresExtracted(Element*, int, int, int)),classifier,SLOT(process(Element*, int, int, int)));
        connect(classifier,SIGNAL(classified(QString,int,int,int)),this,SLOT(finalizeClassification(QString,int,int,int)));

        if(labels == nullptr)
            labels = new QImage(input->size(),QImage::Format_Indexed8);

        labels->setColorTable(inputLabels->colorTable());

        labels->fill(0);
        emit imageReady(labels);

        int step1 = 32;

        leftToCompute = 0;
        for(int step = step1; step >= 1; step -= step * 0.5)
        {

            int prev = 0;
            for(int y = 0; y < labels->height(); y+=step)
            {
                for(int x = 0; x < labels->width(); x+=step)
                {
                    QMutexLocker locker(&lock);
                    int prevY = labels->pixel(x,qMax(0,y-(step*2)));;
                    int next  = labels->pixel(qMin(labels->width()-1,x+(step*2)),y);
                    int nextY = labels->pixel(x,qMin(labels->height()-1,y+(step*2)));;
                    int cur = labels->pixel(x,y);

                    if( cur != next || step == step1 || cur != nextY || cur != prev || cur != prevY)
                    {
                        if(x % (step*2) != 0 || y % (step*2) != 0 || step == step1)
                        {
                            QImage* image = new QImage(32,32,QImage::Format_Grayscale8);
                            for(int xx = 0; xx < 32; xx++)
                            {
                                for(int yy = 0; yy < 32; yy++)
                                {
                                    int x2 = x + xx -16;
                                    int y2 = y + yy -16;

                                    if(x2 >= 0 && x2 < input->width() && y2 >= 0 && y2 < input->height())
                                        image->setPixelColor(xx, yy, input->pixelColor(x2, y2));
                                    else
                                        image->setPixelColor(xx,yy, QColor(0,0,0));

                                }
                            }
                            leftToCompute++;
                            QtConcurrent::run(&classifierThreadPool,extractor,&Extractor::extractFeatures,std::ref(image), QString(), x, y, step);
                        }
                    }
                    locker.unlock();
                    prev = cur;
                }
            }
            while(leftToCompute!=0)
            {

            }
            QMutexLocker locker(&lock);
            emit imageReady(new QImage(*labels));
            locker.unlock();
        }
    }
    emit classifierDone(0);
    emit engineBusy(false);
}

void Engine::saveFFT(QString directory)
{
    emit engineBusy(true);
    for(Class* clss:classes)
    {
        QVector<Element*> elems = clss->getElements();
        QImage pattern(32,32,QImage::Format_Grayscale8);
        for(int x = 0; x < 32; x++)
        {
            for(int y = 0; y < 32; y++)
            {
                double spec;
                for(Element* e:elems)
                {
                    double s = e->getFft()->pixelColor(x,y).red()/255;
                    spec+=s;
                }
                int rgb = spec*255.0/elems.size();
                pattern.setPixelColor(x,y,QColor(rgb,rgb,rgb));
            }
        }
        pattern.save(directory + clss->getLabel(),"bmp");
    }
    emit engineBusy(false);
}

void Engine::changeClassifier(int newClassifier)
{
    currentClassifier = newClassifier;
}

void Engine::changeMetric(int newMetric)
{
    classifiers[0]->setMetric(newMetric);
    classifiers[1]->setMetric(newMetric);
}

void Engine::changeK(int near)
{
    ((KNearestNeighbourClassifier*)classifiers[0])->setK(near);
}

void Engine::addElementToClass(Element *element)
{
    QMutexLocker locker(&lock);

    elements.push_back(element);
    if(!(classes.contains(element->getLabel())))
    {
        classes[element->getLabel()] = new Class(element->getLabel());
    }
    classes[element->getLabel()]->addElement(element);

    trainedItemCount++;


    if(trainedItemCount < trainItemCount)
        emit trainProgress(trainedItemCount,trainItemCount);
    else
    {
        emit engineTrained(classes.size());
//        emit classesReady(classes, elements);
        emit engineBusy(false);
    }
    locker.unlock();

}

void Engine::finalizeClassification(QString className, int x, int y, int step)
{
    QMutexLocker locker(&lock);
    for(int xx = x; xx < x+step; xx++)
    {
        for(int yy = y; yy < y+step; yy++)
        {

            if(xx >= 0 && xx < labels->width() && yy >= 0 && yy < labels->height())
//                labels->setPixel(xx,yy,elementClass);
                labels->setPixel(xx,yy,inputLabels->pixelIndex(x,y));
//            labels->setPixel(xx,yy,classes.keys.indexOf(className));

        }
    }
    leftToCompute--;
    locker.unlock();
}
