#ifndef CLASSVIEWMODEL_H
#define CLASSVIEWMODEL_H

#include "element.h"

#include <QAbstractItemModel>



class ClassViewModel : public QAbstractItemModel
{
public:
    ClassViewModel(QMap<unsigned int, QVector<Element*>> classes);

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    QMap<unsigned int, QVector<Element*>> classes;
};

#endif // CLASSVIEWMODEL_H
