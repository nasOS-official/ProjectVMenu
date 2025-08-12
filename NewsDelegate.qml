import QtQuick
import QtQuick.Controls
import "themes"

Button {
    property alias image: _image.source
    property alias label: _text.text
    property string link: ""

    width: scaleFactor * 275
    height: scaleFactor * 178
    activeFocusOnTab: true
    focus: true


    Rectangle {
        color: Theme.backgroundAlt
        anchors.fill: parent
    }
    Image {
        id: _image
        anchors.fill: parent
        sourceSize.width: 550
        sourceSize.height: 356
        fillMode: Image.PreserveAspectCrop
    }
    Item{
        id: item2
        height: scaleFactor * 45
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        Rectangle {
            id: rectangle
            color: "#d6000000"
            anchors.fill: parent
        }

        Text {
            id: _text
            anchors.leftMargin: 13 * scaleFactor
            anchors.rightMargin: 13 * scaleFactor
            font.pixelSize: 20 * scaleFactor
            verticalAlignment: Text.AlignVCenter
            anchors.fill: parent
            color: "#ffffff"
            clip: true
        }


    }


}
