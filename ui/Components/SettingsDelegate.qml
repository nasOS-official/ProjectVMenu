import QtQuick
import QtQuick.Controls
import Themes 1.0

Button {
    id: root
    background: Rectangle {
        color: Theme.tabbar
        border.color: Theme.button
        border.width: 4 * scaleFactor
    }
    contentItem: Text {
        text: root.text
        font.pixelSize: 20 * scaleFactor
        verticalAlignment: Text.AlignVCenter
        leftPadding: globalSpacing
        color: Theme.text
        anchors.fill: parent
    }
}
