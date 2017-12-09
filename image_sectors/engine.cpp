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
    disconnect(extractor,SIGNAL(featuresExtracted(Element*, int, int, int)));
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
#ifdef QT_DEBUG
            if(item > 0 && itemList.size() < 100)
#endif
#ifndef QT_DEBUG
            if(item > 0)
#endif
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

        Classifier* classifier = classifiers[currentClassifier];

//        classifierThreadPool.setMaxThreadCount(2);

        disconnect(extractor,SIGNAL(featuresExtracted(Element*, int, int, int)),this,SLOT(addElementToClass(Element*)));
        disconnect(classifier,SIGNAL(classified(QString,int,int,int)),this,SLOT(finalizeClassification(QString,int,int,int)));
        disconnect(SIGNAL(stepFinished(int)));

        connect(extractor,SIGNAL(featuresExtracted(Element*, int, int, int)),classifier,SLOT(process(Element*, int, int, int)));
        connect(classifier,SIGNAL(classified(QString,int,int,int)),this,SLOT(finalizeClassification(QString,int,int,int)));
        connect(this,SIGNAL(stepFinished(int)),this,SLOT(adaptiveClassifier(int)));

        if(labels == nullptr)
            labels = new QImage(input->size(),QImage::Format_Indexed8);

        labels->setColorTable(inputLabels->colorTable());

        labels->fill(0);
        step = 64;

        leftToCompute = 0;

        emit stepFinished(step);
    }

}

void Engine::saveFFT(QString directory)
{
    emit engineBusy(true);
    for(Class* clss:classes.values())
    {
        QVector<Element*> elems = clss->getElements();
        QImage ampPattern(64,64,QImage::Format_Grayscale8);
        QImage phaPattern(64,64,QImage::Format_Grayscale8);

        double pixMap[64][64];
        double pixMapP[64][64];
        for(int i = 0; i < elems.size(); i++)
        {
            Element* e = elems[i];
            QString  label = clss->getLabel();

            QImage* amp = e->getFft();
            QImage* pha = e->getPhase();


            for(int x = 0; x < 64; x++)
            {
                for(int y = 0; y < 64; y++)
                {
                    double pix  = qGray(amp->pixelColor(x,y).rgb());
                    double pixP = qGray(pha->pixelColor(x,y).rgb());

                    pixMap[x][y]  += pix;
                    pixMapP[x][y] += pixP;
                }
            }
            amp->save(directory + "\\" + label + "\\amp" + i+32 + ".bmp","bmp");
            pha->save(directory + "\\" + label + "\\pha" + i+32 + ".bmp","bmp");

        }
        double mean = 0;

        double min;
        double minP;
        double max;
        double maxP;

        QMap<double,int> pixels;
        QMap<double,int> pixelsP;

        for(int x = 0; x < 64; x++)
        {
            for(int y = 0; y < 64; y++)
            {
                double pix  = pixMap[x][y];
                double pixP = pixMapP[x][y];
                mean += pix;
                pixels[pix]  = 0;
                pixelsP[pixP] = 0;
            }
        }
        min = pixels.keys()[3];
        max = pixels.keys()[pixels.size()-3];

        minP = pixelsP.keys()[3];
        maxP = pixelsP.keys()[pixelsP.size()-3];

        mean /= ampPattern.height() * ampPattern.width();

        for(int x = 0; x < 64; x++)
        {
            for(int y = 0; y < 64; y++)
            {
//                pixMap[x][y] -= mean - 127;
                pixMap[x][y] = (pixMap[x][y] - min)/(max-min);
                pixMapP[x][y] = (pixMapP[x][y] - minP)/(maxP-minP);
            }
        }
        for(int x = 0; x < 64; x++)
        {
            for(int y = 0; y < 64; y++)
            {
                int rgb = qBound(0.0, (pixMap[x][y])*255, 255.0);
                ampPattern.setPixelColor(x,y,QColor(rgb,rgb,rgb));

                int rgbP = qBound(0.0, (pixMapP[x][y])*255, 255.0);
                phaPattern.setPixelColor(x,y,QColor(rgbP,rgbP,rgbP));
            }
        }
        QString  label = clss->getLabel();

        ampPattern.save(directory + "\\amp_" + label + ".bmp","bmp");
        phaPattern.save(directory + "\\pha_" + label + ".bmp","bmp");
    }
    emit engineBusy(false);
}

void Engine::saveClasses(QString filename)
{

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
        emit classesReady(classes, elements);
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
//                labels->setPixel(xx,yy,inputLabels->pixelIndex(x,y));
            labels->setPixel(xx,yy,classes.keys().indexOf(className));

        }
    }
    leftToCompute--;
    locker.unlock();

    if(leftToCompute==0){
        emit imageReady(new QImage(*labels));
        emit stepFinished(step*0.5);
    }
}

void Engine::adaptiveClassifier(int step)
{
    if(step > 0)
    {
        qDebug() << "Step " << step << " begin";
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
                locker.unlock();

                if( cur != next || step == this->step || cur != nextY || cur != prev || cur != prevY)
                {
                    if(x % (step*2) != 0 || y % (step*2) != 0 || step == this->step)
                    {
                        QImage* image = new QImage(64,64,QImage::Format_ARGB32);
                        for(int xx = 0; xx < 64; xx++)
                        {
                            for(int yy = 0; yy < 64; yy++)
                            {
                                int x2 = x + xx -32;
                                int y2 = y + yy -32;

                                if(x2 >= 0 && x2 < input->width() && y2 >= 0 && y2 < input->height())
                                    image->setPixelColor(xx, yy, input->pixelColor(x2, y2));
                                else
                                    image->setPixelColor(xx,yy, QColor(0,0,0));

                            }
                        }
                        leftToCompute++;
                        //                    extractor->extractFeatures(image,QString(),x,y,step);
                        QtConcurrent::run(extractor,&Extractor::extractFeatures,image, QString(), x, y, step);
                    }
                }
                prev = cur;
            }
        }
    }
    else
    {
        emit engineBusy(false);
        emit classifierDone(0);
    }
}
