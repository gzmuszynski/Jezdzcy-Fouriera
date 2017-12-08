#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "engine.h"

#include <QMainWindow>

namespace Ui {
class UserInterface;
}

class UserInterface : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserInterface(QWidget *parent = 0);
    ~UserInterface();

public slots:

    void extractionProgress(int processed, int final);
    void extractionFinished(int classes);
    void classifierFinished(int recall);

    void imageChanged(QImage* image);
    void lockOptions(bool enabled);

signals:

    void metricChanged(int);
    void kChanged(int);
    void classifierChanged(int);

private slots:
    void on_actionOtw_rz_obraz_triggered();

    void on_pushButton_clicked();

    void on_actionOtw_rz_klasy_triggered();

    void on_actionOtw_rz_zbi_r_testowy_triggered();

    void on_metricSpinBox_valueChanged(int arg1);

    void on_nearSpinBox_valueChanged(int arg1);

    void on_classifierCcomboBox_currentIndexChanged(int index);

    void on_actionZapisz_klasy_triggered();

private:
    Ui::UserInterface *ui;
    Engine* engine;
};

#endif // USERINTERFACE_H
