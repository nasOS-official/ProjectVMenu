import QtQuick
import QtQuick.Controls
import "themes"
import QtQuick.Layouts 2.15



Item {
    id: _root
    RowLayout {
        id: row
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 16 * scaleFactor
        anchors.topMargin: 49 * scaleFactor
        spacing: 8 * scaleFactor

        Text {
            text: qsTr("News")
            font.pixelSize: 32 * scaleFactor
            color: Theme.text
        }

        Button {
            id: button
            height: 30 * scaleFactor
            width: height
            icon.source: "resources/reload.svg"
            icon.height: 20 * scaleFactor
            icon.width: 20 * scaleFactor
            icon.color: Theme.text

            background: Rectangle {
                radius: parent.height / 2
                color: parent.pressed ? Theme.buttonPressed : parent.hovered ? Theme.buttonHovered : Theme.background
            }

            onClicked: {
                newsm.reloadNews();
            }
        }
    }

    Text {
        text: qsTr("My channels")
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 16 * scaleFactor
        anchors.topMargin: 507 * scaleFactor
        font.pixelSize: 32 * scaleFactor
        color: Theme.text
    }
    Text {
        text: channelName
        anchors.top: parent.top
        anchors.topMargin: 49 * scaleFactor
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 32 * scaleFactor
        color: Theme.text
    }
    GridView {
        id: gridView
        visible: !isLoading
        anchors.fill: parent
        anchors.topMargin: 125 * scaleFactor
        anchors.bottomMargin: 200 * scaleFactor
        cellHeight: (178 + 18) * scaleFactor
        cellWidth: (275 + 18) * scaleFactor
        anchors.leftMargin: 18 * scaleFactor
        flow: GridView.TopToBottom
        layoutDirection: Qt.LeftToRight
        highlight: Rectangle {
            z: 20000
            color: "#000000ff"
            border.color: "#00b3ff"
            border.width: 4 * scaleFactor
        }
        activeFocusOnTab: true
        // focus: true
        anchors.rightMargin: 18 * scaleFactor
        model: newsModel
        delegate: NewsDelegate {
            image: model.image
            label: model.label
            link: model.link
            onClicked: {
                gridView.currentIndex = index
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
    GridView {
        id: channelsView
        anchors.fill: parent
        anchors.topMargin: 558 * scaleFactor
        anchors.leftMargin: 16 * scaleFactor
        cellHeight: (142 + 18) * scaleFactor
        // cellHeight: (110 + 18) * scaleFactor
        activeFocusOnTab: true
        // focus: true
        cellWidth: (110 + 18) * scaleFactor
        flow: GridView.TopToBottom
        layoutDirection: Qt.LeftToRight
        model: channelsModel


        highlight: Rectangle {
            z: 20000
            color: "#000000ff"
            border.color: "#00b3ff"
            border.width: 4 * scaleFactor
            radius: height / 2
            height: 110 * scaleFactor
            width: height
        }


        delegate: Item {
            id: item2
            height: 110 * scaleFactor
            width: height
            RoundButton {
                property int iconSize: 30
                width: parent.width
                height: width
                background: Rectangle {
                    color: parent.pressed ? Theme.buttonPressed : parent.hovered ? Theme.buttonHovered : Theme.button
                    radius: height / 2
                }
                // icon.width: iconSize
                // icon.height: iconSize
                // icon.color: Theme.text
                contentItem: Text {
                    text: model.link === "add" ? "+" : model.label[0]
                    font.pixelSize: 32 * scaleFactor
                    font.bold: true
                    color: Theme.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    anchors.fill: parent
                }
                onClicked: {
                    channelsView.currentIndex = index;
                    channelIndex = index;
                    if (model.link === "add"){
                        addChannelLoader.active = true;
                    } else {
                        channelName = model.label;
                        newsm.parseNews(model.link);
                    }
                }
            }

            Text {
                text: model.label
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.bottom
                anchors.leftMargin: 0
                anchors.rightMargin: 0
                anchors.topMargin: 12 * scaleFactor
                font.pixelSize: 16 * scaleFactor
                horizontalAlignment: Text.AlignHCenter
                color: Theme.text
            }
        }
        rebound: Transition {
            NumberAnimation {
                properties: "x"
                duration: 1000
                easing.type: Easing.OutBack
            }
        }

        Component.onCompleted: {
            channelsView.currentIndex = channelIndex;
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: gridView
        height: 250 * scaleFactor
        width: height
        visible: isLoading

    }

}
