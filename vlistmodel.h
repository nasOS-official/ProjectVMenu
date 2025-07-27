#ifndef VLISTMODEL_H
#define VLISTMODEL_H

#include <QAbstractListModel>
#include "velement.h"

class VListModel : public QAbstractListModel
{
    Q_OBJECT
protected:
    QList<VElement> _data;
public:
    enum VListRoles {
        LabelRole = Qt::UserRole + 1,
        ImageRole,
        LinkRole
    };
    explicit VListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    void addData(VElement element);
    int columnCount(const QModelIndex &parent) const;
    void clearData();
};

#endif // VLISTMODEL_H
