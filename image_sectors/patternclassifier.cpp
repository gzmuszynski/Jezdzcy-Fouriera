#include "patternclassifier.h"
#include <QtMath>
#include <QtDebug>

BayesClassifier::BayesClassifier()
{

}

double BayesClassifier::kernel(double x)
{
    double dense = sqrt2pi*qExp(-x*x*0.5);
    return dense;
}

void BayesClassifier::process(Element *element, int x, int y, int step)
{
    normalize(element);

    QMap<QString,double> probability;
    QString best;

    for(QString label:classes.keys())
    {
        double p = 1;
        QVector< QMap<double, double> > dV = this->densities[label];
        QVector<double> features = element->getFeatures();

        for(int f = 0; f < features.size(); f++)
        {
            QMap<double, double> dens = dV[f];

            if(dens.contains(features[f]))
            {
                p+=dens[f];
            }
            else
            {
                dens[features[f]] = -1.0;
                int index = dens.keys().indexOf(features[f]);
                if(index != 0 && index != dens.size()-1)
                {
//                    qDebug() << label << features[f] << dens.keys()[index-1] << dens.keys()[index+1];
                    double mean = (dens[dens.keys()[index-1]] + dens[dens.keys()[index+1]]) * 0.5;
                    if(mean!=0)
                        p *= mean;
                }
                dens.remove(features[f]);
            }
        }

//        p/=features.size();
//        qDebug() << x << y << label << p;

        p*=classes[label]->getElements().size()*1.0/elements.size();
        probability[label] = p;
        if(probability.contains(best))
        {
            best = probability.key(qMax(probability[best],p));
        }
        else
            best = label;
    }

    emit classified(best, x, y, step);
}

void BayesClassifier::setClassElements(QMap<QString, Class *> classes, QVector<Element *> elements)
{
    this->elements = QVector<Element*>(elements);
    normalizeClasses();
    for(Element* e:this->elements)
    {
        QString label = e->getLabel();
        if(!this->classes.contains(label))
        {
            this->classes[label] = new Class(label);
        }
        this->classes[label]->addElement(e);
    }


    QMap<QString,QVector<double>> feats;

    double h = 0.007;

    for(QString label:classes.keys())
    {
        QVector<Element*> elements = this->classes[label]->getElements();
        QVector<double> features = elements[0]->getFeatures();

        QVector<QMap<double, double>> values;
        QVector<QMap<double, double>> dens;

        for(int i = 0; i < elements.size(); i++)
        {
            QVector<double> features = elements[i]->getFeatures();

            for(int f = 0; f < features.size(); f++)
            {
                if(i==0)
                {
                    values.push_back(QMap<double,double>());
                }
                values[f][features[f]]++;

            }
        }
        for(int f = 0; f < features.size(); f++)
        {
            QMap<double, double> densities;
            QList<double> vals = values[f].keys();
            QList<double> valsv = values[f].values();

            for(double x = -0.5; x < 1.5; x += 0.001)
            {
                for(int i = 0; i < values.size(); i++)
                {
                    for(int z = 0; z < valsv[i]; z++)
                    {
                        densities[x]+=kernel((x-vals[i])/h);
                    }
                }
                densities[x]/=elements.size()*h;
                if(densities[x]>1)
                    qDebug() << densities[x];
            }
            dens.push_back(densities);
        }
        this->densities[label] = dens;
    }

}
