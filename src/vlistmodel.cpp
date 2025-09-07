#include "vlistmodel.h"

VListModel::VListModel(QObject *parent)
    : QAbstractListModel{parent}
{}


int VListModel::rowCount(const QModelIndex &) const
{
    return _data.count();
}

int VListModel::columnCount(const QModelIndex &) const
{
    return 1;
}

void VListModel::clearData()
{
    _data.clear();
}



QVariant VListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= _data.count())
        return QVariant();
    const VElement &element = _data.at(index.row());
    switch (role) {
    case LabelRole:
        return element.getLabel();
    case ImageRole:
        return element.getImage();
    case LinkRole:
        return element.getLink();
    case IsStartedRole:
        return element.getIsStarted();
    default:
        return QVariant();

    }
}

QHash<int, QByteArray> VListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[LabelRole] = "label";
    roles[ImageRole] = "image";
    roles[LinkRole] = "link";
    roles[IsStartedRole] = "isStarted";
    return roles;
}

void VListModel::addData(VElement element)
{
    beginResetModel();
    _data.append(element);
    endResetModel();
}

void VListModel::setRunning(int index, bool state){
    VElement &element = _data[index];
    beginResetModel();
    element.setIsStarted(state);
    endResetModel();
}


