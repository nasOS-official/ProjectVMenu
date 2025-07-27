import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import "themes"

Window {
    id: window
    property real scaleFactor: height / 720
    property bool isDarkTheme: true
    property bool isLoading: true
    property int channelIndex: 0
    function loadState(value){
        isLoading = value;
    }

    Component.onCompleted: {
        newsm.isLoading.connect(loadState)
        Theme.setTheme(isDarkTheme)
        newsm.reloadChannels();
        newsm.parseFirst();
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
    Loader {
        id: addChannelLoader
        sourceComponent: AddChannel {
            x: 0
            y: 0
            height: Window.height
            width: Window.width
            onAccepted: {
                newsm.addChannel(label, rss_url);
                newsm.reloadChannels();
                addChannelLoader.active = false;
            }
            onCanceled: {
                // addChannelLoader.destroy()
                addChannelLoader.active = false;
            }
        }
        active: false
    }
}

