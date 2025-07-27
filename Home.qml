import QtQuick
import "themes"

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
        anchors.leftMargin: 18 * scaleFactor
        anchors.rightMargin: 18 * scaleFactor
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
        activeFocusOnTab: true
        focus: true

        delegate: GameDelegate {
            image: model.image
            label: model.label
            executable: model.link
            onClicked: {
                listView.currentIndex = index;
                _vrunner.buttonClicked(executable);
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
