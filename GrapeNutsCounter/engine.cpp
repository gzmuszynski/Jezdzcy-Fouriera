#include "engine.h"
#include "extractor.h"
#include <QImage>

Engine::Engine(QObject *parent) : QObject(parent)
{
}

void Engine::countObjects(QString filename)
{
    QImage *image = new QImage(filename);

    if(!image->isNull())
    {
        extractor = new Extractor(this);
        connect(extractor,SIGNAL(featuresExtracted(QImage*)),this,SLOT(receiveImage(QImage*)));

        extractor->extractFeatures(image);
    }
}

void Engine::receiveImage(QImage *image)
{
    emit imageReady(image);
}
