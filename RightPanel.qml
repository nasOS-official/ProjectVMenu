import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "themes"

Item {
    id: rightPanel
    property int btnSize: 80 * scaleFactor
    width: visible ? 115 * scaleFactor : 0
    property int currentTab: 1

    Rectangle {
        id: rectangle
        color: Theme.tabbar
        anchors.fill: parent
    }

    MouseArea {
        anchors.fill: parent
    }

    ListModel {
        id: rightModel
        ListElement { icon: "";}
        ListElement { icon: "resources/home.svg";}
        ListElement { icon: "resources/shop.svg";}
        ListElement { icon: "resources/news.svg";}
        ListElement { icon: "resources/devices.svg";}
        ListElement { icon: "resources/settings.svg";}
        ListElement { icon: "resources/power.svg";}
    }

    // Shortcut {
    //     sequence: "Escape"
    //     onActivated: {
    //         if (currentTab != 1){
    //             stackView.replace("qrc:/Home.qml");
    //             currentTab = 1;
    //             rightView.currentIndex = 1;
    //             // rightPanel.visible = true;
    //         }
    //     }
    // }

    GridView {
        id: rightView
        width: btnSize

        boundsBehavior: Flickable.StopAtBounds
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: 26 * scaleFactor
        anchors.horizontalCenter: parent.horizontalCenter
        model: rightModel

        highlight: Rectangle {
            z: 20000
            color: "#000000ff"
            border.color: "#00b3ff"
            border.width: 4 * scaleFactor
            radius: height / 2
        }

        cellWidth: btnSize
        cellHeight: btnSize + globalSpacing

        delegate: TabSideButton {

            size: btnSize
            iconSource: model.icon
            onClicked: {
                if (index != currentTab){
                    switch (index){
                    case 1:
                        stackView.replace("qrc:/Home.qml");
                        break;
                    case 2:
                        stackView.replace("qrc:/Shop.qml");
                        break;
                    case 3:
                        stackView.replace("qrc:/News.qml");
                        break;
                    case 4:
                        stackView.replace("qrc:/Devices.qml");
                        break;
                    case 5:
                        stackView.replace("qrc:/Settings.qml");
                        break;
                    case 6:
                        Qt.callLater(Qt.quit);
                        break;
                    }
                    if (index > 0){
                        // rightPanel.visible = false;
                        currentTab = index;
                        rightView.currentIndex = index;
                    }
                }
            }
        }
    }
    Component.onCompleted: {
        rightView.currentIndex = currentTab;
    }
}
