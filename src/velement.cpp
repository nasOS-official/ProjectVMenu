#include "velement.h"


VElement::VElement(const QString &label, const QString &image, const QString &link)
    : _label(label), _image(image), _link(link) {}

QString VElement::getLabel() const { return _label; }

QString VElement::getImage() const { return _image; }

QString VElement::getLink() const { return _link; }

bool VElement::getIsStarted() const { return _isStarted; }

void VElement::setIsStarted(bool state) { _isStarted = state; }
