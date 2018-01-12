#include "counterwindow.h"
#include "ui_counterwindow.h"
#include <QFileDialog>

CounterWindow::CounterWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CounterWindow)
{
    ui->setupUi(this);
    engine = new Engine();
    connect(this,SIGNAL(runRecognition(QString)),engine,SLOT(countObjects(QString)));
    ui->graphicsView->setScene(new QGraphicsScene());
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

void CounterWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,"Otwórz plik do przeliczenia", "../", "Obrazy (*.BMP *.JPG *PNG)");

    if(!filename.isEmpty())
    {
        connect(engine,SIGNAL(imageReady(QImage*)),this,SLOT(setImage(QImage*)));
        emit runRecognition(filename);
    }
}

void CounterWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(),Qt::KeepAspectRatio);

}
