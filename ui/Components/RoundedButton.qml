import QtQuick
import QtQuick.Controls
import Themes 1.0

Button {
    id: root
    property real radius: height / 2

    background: Rectangle {
        radius: root.radius
        color: root.pressed ? Theme.buttonPressed : root.hovered ? Theme.buttonHovered : Theme.button
    }

    contentItem: Text {
        text: root.text
        font.pixelSize: root.font.pixelSize
        font.bold: root.font.bold
        color: Theme.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.fill: parent
    }

}
