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
    void runRecognition();
    void fileReady(QString filename);

    void dialsChanged(int rad1, int rad2, int increment, int mapSize);

public slots:
    void setImage(QImage* image);
    void setClasses(QMap<unsigned int, QVector<Element*>> classes);

private slots:
    void on_actionOpen_triggered();

    void on_pushButton_clicked();

    void on_minSpinBox_valueChanged(int arg1);

    void on_maxSpinBox_valueChanged(int arg1);

    void on_incrementSpinBox_valueChanged(int arg1);

    void on_mapSizeSpinBox_valueChanged(int arg1);

    void on_actionSave_triggered();

    void on_actionSaveData_triggered();

private:
    Ui::CounterWindow *ui;
    Engine* engine;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // COUNTERWINDOW_H
