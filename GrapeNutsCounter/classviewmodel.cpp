#include "classviewmodel.h"

#include <QBrush>


ClassViewModel::ClassViewModel(QMap<unsigned int, QVector<Element *> > elements): classes(elements)
{

}

QModelIndex ClassViewModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row,column);
}

QModelIndex ClassViewModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int ClassViewModel::rowCount(const QModelIndex &parent) const
{
    return classes.size();
}

int ClassViewModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant ClassViewModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole){
        return QString("%1").arg(classes.values()[index.row()].size());
    }
    if(role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }
    if(role == Qt::BackgroundRole)
    {
        return QBrush(QColor::fromRgba(classes.keys()[index.row()]));
    }
    if(role == Qt::ForegroundRole)
    {
        if(qGray(QColor::fromRgba(classes.keys()[index.row()]).rgb()) < 128)
        {
            return QBrush(QColor(255,255,255));
        }
        else
        {
            return QBrush(QColor(0,0,0));
        }
    }
    return QVariant();
}
