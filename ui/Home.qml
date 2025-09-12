import QtQuick
import Themes
import Components
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Item {
    id: item1


    RowLayout {
        id: row
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 16 * scaleFactor
        anchors.topMargin: 49 * scaleFactor
        spacing: 8 * scaleFactor


        Text {
            text: qsTr("Home")
            font.pixelSize: 32 * scaleFactor
            color: Theme.text
        }

        Button {
            height: 30 * scaleFactor
            width: height
            icon.source: "qrc:/resources/close.svg"
            icon.height: 20 * scaleFactor
            icon.width: 20 * scaleFactor
            icon.color: Theme.text

            background: Rectangle {
                radius: parent.height / 2
                color: parent.pressed ? Theme.buttonPressed : parent.hovered ? Theme.buttonHovered : Theme.background
            }

            onClicked: {
                _vrunner.terminate();
            }
        }
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
