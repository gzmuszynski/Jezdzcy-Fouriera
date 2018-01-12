#ifndef COUNTERWINDOW_H
#define COUNTERWINDOW_H

#include "engine.h"

#include <QMainWindow>

namespace Ui {
class CounterWindow;
}

class CounterWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CounterWindow(QWidget *parent = 0);
    ~CounterWindow();

signals:
    void runRecognition(QString filename);

public slots:
    void setImage(QImage* image);

private slots:
    void on_actionOpen_triggered();

private:
    Ui::CounterWindow *ui;
    Engine* engine;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // COUNTERWINDOW_H
