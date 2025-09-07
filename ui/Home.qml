import QtQuick
import Themes
import Components

Item {
    id: item1

    Text {
        text: qsTr("Home")
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 16 * scaleFactor
        anchors.topMargin: 49 * scaleFactor
        font.pixelSize: 32 * scaleFactor
        color: Theme.text
    }
    GridView {
        id: listView
        anchors.fill: parent
        anchors.topMargin: 125 * scaleFactor
        anchors.leftMargin: globalSpacing
        anchors.rightMargin: globalSpacing
        // anchors.bottomMargin: 200 * scaleFactor
        cellWidth: (275 + 18) * scaleFactor
        cellHeight: cellWidth
        flow: GridView.TopToBottom
        layoutDirection: Qt.LeftToRight
        model: appsModel
        highlight: Rectangle {
            z: 20000
            color: "#000000ff"
            border.color: "#00b3ff"
            border.width: 4 * scaleFactor
        }

        delegate: GameDelegate {
            id: _gameDelegate
            image: model.image
            label: model.label
            executable: model.link
            playing: model.isStarted

            onClicked: {
                listView.currentIndex = index;
                if (_vrunner.isRunning() === false){
                    _vrunner.start(executable, index);
                } else if (!exitAppLoader.active) {
                    _vrunner.startRequest(executable, index);
                    exitAppLoader.active = true;
                }
            }
        }

        rebound: Transition {
            NumberAnimation {
                properties: "x"
                duration: 1000
                easing.type: Easing.OutBack
            }
        }
    }
}
