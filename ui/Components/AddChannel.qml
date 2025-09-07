import QtQuick
import Themes 1.0
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    signal accepted
    signal canceled
    property alias label: label.text
    property alias rss_url: rss_url.text

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
            color: Theme.text
            text: qsTr("If you want to add a channel, you need to write the channel name and URL.")
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.leftMargin: 8 * scaleFactor
            anchors.rightMargin: 8 * scaleFactor
            anchors.topMargin: 32 * scaleFactor
            font.pixelSize: 24 * scaleFactor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
        }

        Column {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 8 * scaleFactor
            anchors.rightMargin: 8 * scaleFactor
            anchors.bottomMargin: (64 + 16) * scaleFactor
            spacing: 16 * scaleFactor

            RowLayout {
                id: row1
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 0
                anchors.rightMargin: 0
                Text {
                    id: text1
                    Layout.preferredWidth: parent.width / 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Channel name:")
                    font.pixelSize: 24 * scaleFactor
                    color: Theme.text
                }

                TextInput {
                    id: label
                    Layout.preferredWidth: parent.width / 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignHCenter
                    text: "Example"
                    font.pixelSize: 24 * scaleFactor
                    validator: RegularExpressionValidator {
                        regularExpression: /[^:]*/gm
                    }
                    rightPadding: 8 * scaleFactor
                    bottomPadding: 8 * scaleFactor
                    leftPadding: 8 * scaleFactor
                    topPadding: 8 * scaleFactor
                    color: Theme.text
                    clip: true
                    Rectangle {
                        radius: height / 4
                        anchors.fill: parent
                        color: parent.pressed ? Theme.buttonPressed : parent.hovered ? Theme.buttonHovered : Theme.button
                        z: parent.z - 1
                    }
                }
            }

            // Row {
            RowLayout {
                id: row2
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 0
                anchors.rightMargin: 0
                Text {
                    id: text2
                    Layout.preferredWidth: parent.width / 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Channel URL:")
                    font.pixelSize: 24 * scaleFactor
                    color: Theme.text
                }

                TextInput {
                    id: rss_url
                    Layout.preferredWidth: parent.width / 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignHCenter
                    text: "example.com/rss.xml"
                    font.pixelSize: 24 * scaleFactor
                    rightPadding: 8 * scaleFactor
                    bottomPadding: 8 * scaleFactor
                    leftPadding: 8 * scaleFactor
                    topPadding: 8 * scaleFactor
                    color: Theme.text
                    clip: true
                    Rectangle {
                        radius: height / 4
                        anchors.fill: parent
                        color: parent.pressed ? Theme.buttonPressed : parent.hovered ? Theme.buttonHovered : Theme.button
                        z: parent.z - 1
                    }
                }
            }
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
