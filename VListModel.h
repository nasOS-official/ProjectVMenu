#include <QString>
#include <QByteArray>
#include <QAbstractListModel>

#ifndef VLISTMODEL_H
#define VLISTMODEL_H

class VElement
{
public:
    VElement(const QString &label, const QString &image, const QString &link)
        : _label(label), _image(image), _link(link) {}

    QString getLabel() const { return _label; }
    QString getImage() const { return _image; }
    QString getLink() const { return _link; }
private:
    QString _label;
    QString _image;
    QString _link;
};

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

    VListModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const {
        QHash<int, QByteArray> roles;
        roles[LabelRole] = "label";
        roles[ImageRole] = "image";
        roles[LinkRole] = "link";
        return roles;
    }

    int rowCount(const QModelIndex &parent) const
    {
        return _data.count();
    }
    QVariant data(const QModelIndex &index, int role) const
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
            default:
                return QVariant();

        }

    }
    void addData(VElement element){
        _data.append(element);
    }
};



#endif // VLISTMODEL_H
