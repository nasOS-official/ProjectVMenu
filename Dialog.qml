import QtQuick
import "themes"
import QtQuick.Controls

Item {
    id: root
    width: 800 * scaleFactor
    height: 300 * scaleFactor
    property alias text: _text.text
    signal clicked

    Rectangle {
        id: rectangle
        // color: Theme.tabbar
        color: Theme.backgroundAlt
        radius: 40 * scaleFactor
        anchors.fill: parent
        clip: false
    }

    Text {
        id: _text
        height: 19
        color: Theme.text
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        font.pixelSize: 24 * scaleFactor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenterOffset: -32 * scaleFactor

    }

    RoundedButton {
        id: button
        height: 64 * scaleFactor
        text: qsTr("Ok")
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 8 * scaleFactor
        anchors.rightMargin: 8 * scaleFactor
        anchors.bottomMargin: 8 * scaleFactor
        font.pixelSize: 24 * scaleFactor
        onClicked: {
            root.clicked();
        }
    }

}
