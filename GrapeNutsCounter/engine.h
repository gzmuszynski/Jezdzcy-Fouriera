#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>

class Extractor;

class Engine : public QObject
{
    Q_OBJECT
public:
    explicit Engine(QObject *parent = nullptr);

signals:
    void objectsCounted(int count);
    void imageReady(QImage* image);

public slots:
    void countObjects(QString filename);
    void receiveImage(QImage * image);

private:
    Extractor* extractor;
};

#endif // ENGINE_H
