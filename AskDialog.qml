import QtQuick
import "themes"
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    property alias text: _text.text
    signal accepted
    signal canceled

    Rectangle{
        opacity: 0.7
        color: "black"
        anchors.fill: parent
    }
    MouseArea {
        anchors.fill: parent
    }

    Item {
        width: 800 * scaleFactor
        height: 300 * scaleFactor
        anchors.centerIn: parent

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
            anchors.leftMargin: 8 * scaleFactor
            anchors.rightMargin: 8 * scaleFactor
            font.pixelSize: 24 * scaleFactor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenterOffset: -32 * scaleFactor
            wrapMode: Text.WordWrap
        }

        RowLayout {
            id: row
            height: 64 * scaleFactor
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 8 * scaleFactor
            anchors.rightMargin: 8 * scaleFactor
            anchors.bottomMargin: 8 * scaleFactor
            spacing: 8 * scaleFactor

            RoundedButton {
                id: roundButton
                text: qsTr("Ok")
                font.pixelSize: 24 * scaleFactor
                Layout.preferredWidth: parent.width / 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignHCenter
                onClicked: root.accepted()
            }

            RoundedButton {
                id: roundButton1
                text: qsTr("Cancel")
                font.pixelSize: 24 * scaleFactor
                Layout.preferredWidth: parent.width / 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignHCenter
                onClicked: root.canceled()
            }
        }
    }
}
