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
    ListView {
        id: listView
        anchors.fill: parent
        anchors.topMargin: 125 * scaleFactor
        anchors.leftMargin: 18 * scaleFactor
        anchors.rightMargin: 18 * scaleFactor
        orientation: ListView.Horizontal
        spacing: 18 * scaleFactor
        model: listModel
        delegate: GameDelegate {
            image: model.image
            label: model.label

        }

        rebound: Transition {
            NumberAnimation {
                properties: "x"
                duration: 1000
                easing.type: Easing.OutBack
            }
        }
    }
    ListModel {
        id: listModel
        ListElement {
            image: "https://supertuxkart.net/assets/images/header.jpg"
            label: "SuperTuxCart"
        }
        ListElement {
            image: "https://supertuxkart.net/assets/images/header.jpg"
            label: "SuperTuxCart"
        }
        ListElement {
            image: "https://supertuxkart.net/assets/images/header.jpg"
            label: "SuperTuxCart"
        }
        ListElement {
            image: "https://supertuxkart.net/assets/images/header.jpg"
            label: "SuperTuxCart"
        }
        ListElement {
            image: "https://supertuxkart.net/assets/images/header.jpg"
            label: "SuperTuxCart"
        }
        ListElement {
            image: "https://supertuxkart.net/assets/images/header.jpg"
            label: "SuperTuxCart"
        }
        ListElement {
            image: "https://supertuxkart.net/assets/images/header.jpg"
            label: "SuperTuxCart"
        }
        ListElement {
            image: "https://supertuxkart.net/assets/images/header.jpg"
            label: "SuperTuxCart"
        }
        ListElement {
            image: "https://supertuxkart.net/assets/images/header.jpg"
            label: "SuperTuxCart"
        }
        ListElement {
            image: "https://supertuxkart.net/assets/images/header.jpg"
            label: "SuperTuxCart"
        }
        ListElement {
            image: "https://supertuxkart.net/assets/images/header.jpg"
            label: "SuperTuxCart"
        }
    }
    // GameDelegate {
    //     image: "https://supertuxkart.net/assets/images/header.jpg"
    //     label: "SuperTuxCart"
    // }
}
