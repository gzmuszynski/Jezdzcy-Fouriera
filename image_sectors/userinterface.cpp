#include "userinterface.h"
#include "ui_userinterface.h"
#include <engine.h>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QtConcurrent/QtConcurrent>

UserInterface::UserInterface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UserInterface)
{
    ui->setupUi(this);
    engine = new Engine();

    connect(this,SIGNAL(metricChanged(int)),engine,SLOT(changeMetric(int)));
    connect(this,SIGNAL(kChanged(int)),engine,SLOT(changeK(int)));
    connect(this,SIGNAL(classifierChanged(int)),engine,SLOT(changeClassifier(int)));
    connect(engine,SIGNAL(engineBusy(bool)),this,SLOT(lockOptions(bool)));
}

UserInterface::~UserInterface()
{
    delete ui;
}

void UserInterface::extractionProgress(int processed, int final)
{
    ui->statusBar->showMessage(QString("Przetworzono %1 z %2 obrazów").arg(processed).arg(final));
}

void UserInterface::extractionFinished(int classes)
{
    ui->statusBar->showMessage(QString("Wczytano %1 klas").arg(classes) + (classes==1? "ę":""));
    qDebug() << "Extraction completed:" << classes << "classes found";
}

void UserInterface::classifierFinished(int recall)
{
    ui->statusBar->showMessage(QString("Zakończono klasyfikowanie. Osiągnięto skuteczność %1\%").arg(recall));
    qDebug() << "Classification completed:" << recall << "% accuracy";

}

void UserInterface::imageChanged(QImage *image)
{
    ui->imageView->setScene(new QGraphicsScene());
    ui->imageView->scene()->addItem(new QGraphicsPixmapItem(QPixmap::fromImage(*image)));
//    ui->statusBar->showMessage(QString("Image"));
}

void UserInterface::lockOptions(bool enabled)
{
    enabled = !enabled;
    ui->pushButton                ->setEnabled(enabled);
    ui->metricSpinBox             ->setEnabled(enabled);
    ui->classifierCcomboBox       ->setEnabled(enabled);
    ui->nearSpinBox               ->setEnabled(enabled);
    ui->actionOtw_rz_klasy        ->setEnabled(enabled);
    ui->actionOtw_rz_zbi_r_testowy->setEnabled(enabled);
    ui->actionZapisz_klasy        ->setEnabled(enabled);
    ui->actionPoka_b_edy          ->setEnabled(enabled);
    ui->actionPoka_testowany      ->setEnabled(enabled);
    ui->actionPoka_wynik          ->setEnabled(enabled);
    if(enabled)
    {
        double time = begin.msecsTo(QTime::currentTime())*0.001;
        qDebug() << "Execute Time: " << time << "seconds";
    }
}

void UserInterface::on_actionOtw_rz_obraz_triggered()
{
    QString image = QFileDialog::getOpenFileName(this,"Otwórz obraz testowy","../sectors","*.BMP");
    if(image.isEmpty())
        return;
    QString labels = QFileDialog::getOpenFileName(this,"Otwórz obraz z etykietami",QString(),"*.BMP");

    connect(engine,SIGNAL(imageReady(QImage*)),this,SLOT(imageChanged(QImage*)));

    if(!labels.isEmpty())
        QtConcurrent::run(QThreadPool::globalInstance(),engine,&Engine::loadImage,image,labels);

}

void UserInterface::on_pushButton_clicked()
{
    connect(engine,SIGNAL(imageReady(QImage*)),this,SLOT(imageChanged(QImage*)));
    connect(engine,SIGNAL(classifierDone(int)),this,SLOT(classifierFinished(int)));

    begin = QTime::currentTime();
    QtConcurrent::run(QThreadPool::globalInstance(),engine,&Engine::classify);
}

void UserInterface::on_actionOtw_rz_klasy_triggered()
{
    begin = QTime::currentTime();
    QString dir = QFileDialog::getOpenFileName(this,"Zapisz do pliku","../sectors");

    if(!dir.isEmpty())
    {
        connect(engine,SIGNAL(engineTrained(int)),this,SLOT(extractionFinished(int)));
        QtConcurrent::run(QThreadPool::globalInstance(),engine,&Engine::openClasses,dir);
    }
}

void UserInterface::on_actionOtw_rz_zbi_r_testowy_triggered()
{
    begin = QTime::currentTime();
    QString dir = QFileDialog::getExistingDirectory(this,"Otwórz katalog","../sectors");

    if(!dir.isEmpty())
    {
        connect(engine,SIGNAL(trainProgress(int,int)),this,SLOT(extractionProgress(int,int)));
        connect(engine,SIGNAL(engineTrained(int)),this,SLOT(extractionFinished(int)));

        QtConcurrent::run(QThreadPool::globalInstance(),engine,&Engine::train,dir);
    }
}

void UserInterface::on_metricSpinBox_valueChanged(int arg1)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    if(arg1 == 0)
        ui->metricSpinBox->setSpecialValueText("\xE2\x88\x9E");

    emit metricChanged(arg1);
}

void UserInterface::on_nearSpinBox_valueChanged(int arg1)
{
    emit kChanged(arg1);
}

void UserInterface::on_classifierCcomboBox_currentIndexChanged(int index)
{
    emit classifierChanged(index);
    if(index == 0)
    {
        ui->label_2->setVisible(true);
        ui->nearSpinBox->setVisible(true);
    }
    else
    {
        ui->label_2->setVisible(false);
        ui->nearSpinBox->setVisible(false);
    }
}

void UserInterface::on_actionZapisz_klasy_triggered()
{
    begin = QTime::currentTime();
    QString dir = QFileDialog::getSaveFileName(this,"Zapisz do pliku","../sectors");
//    QString dir = QFileDialog::getExistingDirectory(this,"Zapisz do pliku","../sectors");

    if(!dir.isEmpty())
    {
        QtConcurrent::run(QThreadPool::globalInstance(),engine,&Engine::saveClasses,dir);
    }
}

void UserInterface::on_actionPoka_b_edy_triggered()
{
    connect(engine,SIGNAL(imageReady(QImage*)),this,SLOT(imageChanged(QImage*)));

    QtConcurrent::run(QThreadPool::globalInstance(),engine,&Engine::showError);
}

void UserInterface::on_actionPoka_wynik_triggered()
{
    connect(engine,SIGNAL(imageReady(QImage*)),this,SLOT(imageChanged(QImage*)));

    QtConcurrent::run(QThreadPool::globalInstance(),engine,&Engine::showLabel);
}

void UserInterface::on_actionPoka_testowany_triggered()
{
    connect(engine,SIGNAL(imageReady(QImage*)),this,SLOT(imageChanged(QImage*)));

    QtConcurrent::run(QThreadPool::globalInstance(),engine,&Engine::showInput);
}

void UserInterface::on_actionZapisz_obraz_triggered()
{
    QString dir = QFileDialog::getSaveFileName(this,"Zapisz do pliku","../sectors","*.bmp");

    if(!dir.isEmpty())
    {
        QtConcurrent::run(QThreadPool::globalInstance(),engine,&Engine::saveImage,dir);
    }
}
