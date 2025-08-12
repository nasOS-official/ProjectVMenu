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
    property string channelName: ""
    function loadState(value){
        isLoading = value;
    }

    function stateChanged(value){
        appLoading.visible = value === 1 ? true : false;

    }

    Component.onCompleted: {
        newsm.isLoading.connect(loadState);
        _vrunner.stateChanged.connect(stateChanged);
        Theme.setTheme(isDarkTheme);
        newsm.reloadChannels();
        channelName = newsm.parseFirst();
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
            timeLabel.text = Qt.formatTime(new Date(), "hh:mm");
            gc();
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
                channelName = newsm.parseLast();
                addChannelLoader.active = false;
            }
            onCanceled: {
                addChannelLoader.active = false;
            }
        }
        active: false
    }

    Loader {
        id: exitAppLoader
        property string execs: ""
        property int index: 0
        sourceComponent: AskDialog {
            id: _dialog
            x: 0
            y: 0
            height: Window.height
            width: Window.width
            text: qsTr("The %1 is already running. To terminate the application and start a new one, press ok.").arg(_vrunner.currentApp())
            onAccepted: {
                _vrunner.terminate();
                _vrunner.acceptStartRequest();
                exitAppLoader.active = false;
            }
            onCanceled: {
                exitAppLoader.active = false;
            }
        }
        active: false
    }

    Item {
        id: appLoading
        anchors.fill: parent
        visible: false
        MouseArea {
            anchors.fill: parent
        }
        Rectangle {
            anchors.fill: parent
            color: "black"
        }

        BusyIndicator {
            anchors.centerIn: parent
            height: 250 * scaleFactor
            width: height
        }
    }
}

