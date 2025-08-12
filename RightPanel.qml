import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "themes"

Item {
    id: rightPanel
    property int btnSize: 80 * scaleFactor
    width: 115 * scaleFactor
    property int currentTab: 0

    Rectangle {
        id: rectangle
        color: Theme.tabbar
        anchors.fill: parent
    }

    MouseArea {
        anchors.fill: parent
    }

    Column {
        id: columnLayout
        anchors.fill: parent
        anchors.topMargin: 26 * scaleFactor
        spacing: 18 * scaleFactor

        TabSideButton {
            id: roundButton1
            size: btnSize
            anchors.horizontalCenter: parent.horizontalCenter
        }

        TabSideButton {
            id: roundButton2
            size: btnSize
            iconSource: "resources/home.svg"
            onClicked: {
                if (0 != currentTab){
                    stackView.replace(Qt.createComponent("qrc:/Home.qml").createObject());
                    currentTab = 0;
                }
            }
            anchors.horizontalCenter: parent.horizontalCenter
        }

        TabSideButton {
            id: roundButton3
            size: btnSize
            iconSource: "resources/shop.svg"
            onClicked: {
                if (1 != currentTab){
                    stackView.replace(Qt.createComponent("qrc:/Shop.qml").createObject());
                    currentTab = 1;
                }
            }
            anchors.horizontalCenter: parent.horizontalCenter
        }

        TabSideButton {
            id: roundButton4
            size: btnSize
            iconSource: "resources/news.svg"
            onClicked: {
                if (2 != currentTab){
                    stackView.replace(Qt.createComponent("qrc:/News.qml").createObject());
                    currentTab = 2;
                }
            }
            anchors.horizontalCenter: parent.horizontalCenter
        }

        TabSideButton {
            id: roundButton5
            size: btnSize
            iconSource: "resources/devices.svg"
            onClicked: {
                if (3 != currentTab){
                    stackView.replace(Qt.createComponent("qrc:/Devices.qml").createObject());
                    currentTab = 3;
                }
            }
            anchors.horizontalCenter: parent.horizontalCenter
        }

        TabSideButton {
            id: roundButton6
            size: btnSize
            iconSource: "resources/settings.svg"
            onClicked: {
                if (4 != currentTab){
                    stackView.replace(Qt.createComponent("qrc:/Settings.qml").createObject());
                    currentTab = 4;
                }
            }
            anchors.horizontalCenter: parent.horizontalCenter
        }

        TabSideButton {
            id: roundButton7
            size: btnSize
            iconSource: "resources/power.svg"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                Qt.callLater(Qt.quit)
            }
        }
    }
}
