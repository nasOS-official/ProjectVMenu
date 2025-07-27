#ifndef VELEMENT_H
#define VELEMENT_H
#include <QString>

class VElement
{
public:
    VElement(const QString &label, const QString &image, const QString &link);

    QString getLabel() const;
    QString getImage() const;
    QString getLink() const;
private:
    QString _label;
    QString _image;
    QString _link;
};

#endif // VELEMENT_H
