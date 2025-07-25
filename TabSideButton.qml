import QtQuick
import QtQuick.Controls
import "themes"

RoundButton {
    id: root
    property alias size: root.width
    property int iconSize: 30
    property alias iconSource: root.icon.source

    height: width
    background: Rectangle {
        color: root.pressed ? Theme.buttonPressed : root.hovered ? Theme.buttonHovered : Theme.button
        radius: height / 2
    }
    icon.width: iconSize
    icon.height: iconSize
    icon.color: Theme.text
}
