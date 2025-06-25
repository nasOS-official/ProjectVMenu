import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import "themes"

Window {
    id: window
    property real scaleFactor: height / 720
    property bool isDarkTheme: true

    Component.onCompleted: {
        Theme.setTheme(isDarkTheme)
    }
    width: 1280
    height: 720
    visible: true
    color: Theme.background
    title: qsTr("PVMenu")

    StackView {
        id: stackView
        anchors.left: rightPanel.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        initialItem: Loader {
            source: "Home.qml"

        }
        replaceEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                easing.type: Easing.InCubic
                from: 0
                to:1
                duration: 200
            }
        }
        replaceExit: Transition {
            ParallelAnimation {
                PropertyAnimation {
                    property: "scale"
                    from: 1.0
                    to: 0.8
                    duration: 200
                    easing.type: Easing.InCubic
                }
                PropertyAnimation {
                    property: "opacity"
                    from: 1.0
                    to: 0.0
                    duration: 200
                    easing.type: Easing.InCubic
                }
            }
        }
    }

    RightPanel {
        id: rightPanel
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.bottomMargin: 0
    }
    Text {
        id: timeLabel
        text: {
            Qt.formatDateTime(new Date(), "hh:mm")
        }
        color: Theme.text
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 60 * scaleFactor
        anchors.topMargin: 49 * scaleFactor
        font.pixelSize: 32 * scaleFactor
    }
    Timer {
        id: timeUpdater
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            timeLabel.text = Qt.formatTime(new Date(), "hh:mm")
        }
    }
}

