#include "classviewmodel.h"
#include "counterwindow.h"
#include "ui_counterwindow.h"
#include <QFileDialog>

CounterWindow::CounterWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CounterWindow)
{
    ui->setupUi(this);
    engine = new Engine();
    ui->graphicsView->setScene(new QGraphicsScene());
    connect(this,SIGNAL(fileReady(QString)),engine,SLOT(loadImage(QString)));
    connect(this,SIGNAL(dialsChanged(int,int,int,int)),engine,SLOT(changeConfiguration(int,int,int,int)));
}

CounterWindow::~CounterWindow()
{
    delete ui;
}

void CounterWindow::setImage(QImage *image)
{
    ui->graphicsView->setScene(new QGraphicsScene());
    ui->graphicsView->scene()->addPixmap(QPixmap::fromImage(*image));
    ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(),Qt::KeepAspectRatio);
}

void CounterWindow::setClasses(QMap<unsigned int, QVector<Element *> > classes)
{
    ui->tableView->setModel(new ClassViewModel(classes));
    ui->tableView->horizontalHeader()->setVisible(false);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
}

void CounterWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,"Otwórz plik do przeliczenia", "../", "Obrazy (*.BMP *.JPG *PNG)");

    if(!filename.isEmpty())
    {
        disconnect(this,SIGNAL(fileReady(QString)),engine,SLOT(saveImage(QString)));
        connect(this,SIGNAL(fileReady(QString)),engine,SLOT(loadImage(QString)));
        disconnect(this,SIGNAL(fileReady(QString)),engine,SLOT(saveData(QString)));

        connect(this,SIGNAL(runRecognition()),engine,SLOT(countObjects()));
        connect(engine,SIGNAL(imageReady(QImage*)),this,SLOT(setImage(QImage*)));
        emit fileReady(filename);
    }
}

void CounterWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(),Qt::KeepAspectRatio);

}

void CounterWindow::on_pushButton_clicked()
{
    connect(engine,SIGNAL(classesReady(QMap<uint,QVector<Element*> >)),this,SLOT(setClasses(QMap<uint,QVector<Element*> >)));
    emit runRecognition();
}

void CounterWindow::on_minSpinBox_valueChanged(int arg1)
{
    emit dialsChanged(arg1, 0, 0, 0);
}

void CounterWindow::on_maxSpinBox_valueChanged(int arg1)
{
    emit dialsChanged(0, arg1, 0, 0);
}

void CounterWindow::on_incrementSpinBox_valueChanged(int arg1)
{
    emit dialsChanged(0, 0, arg1, 0);
}

void CounterWindow::on_mapSizeSpinBox_valueChanged(int arg1)
{
    emit dialsChanged(0, 0, 0, arg1);
}

void CounterWindow::on_actionSave_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this,"Zapisz obraz", "../", "Obrazy (*.BMP *.JPG *PNG)");


    disconnect(this,SIGNAL(fileReady(QString)),engine,SLOT(loadImage(QString)));
    connect(this,SIGNAL(fileReady(QString)),engine,SLOT(saveImage(QString)));
    disconnect(this,SIGNAL(fileReady(QString)),engine,SLOT(saveData(QString)));

    if(!filename.isEmpty())
    {
        emit fileReady(filename);
    }
}

void CounterWindow::on_actionSaveData_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this,"Zapisz dane o klasach", "../", "Attribute-Relation File Format (*.ARFF)");


    disconnect(this,SIGNAL(fileReady(QString)),engine,SLOT(loadImage(QString)));
    disconnect(this,SIGNAL(fileReady(QString)),engine,SLOT(saveImage(QString)));
    connect(this,SIGNAL(fileReady(QString)),engine,SLOT(saveData(QString)));

    if(!filename.isEmpty())
    {
        emit fileReady(filename);
    }
}
