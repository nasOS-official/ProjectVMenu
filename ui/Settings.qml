import QtQuick
import QtCore
import Themes
import QtQuick.Controls
import QtQuick.Layouts
import QtQml.Models
import QtQuick.Window
import Components

Item {

    RowLayout {
        id: column
        anchors.fill: parent
        anchors.leftMargin: globalSpacing
        anchors.rightMargin: globalSpacing
        anchors.topMargin: 124 * scaleFactor
        spacing: globalSpacing
        ListModel {
            id: tabModel
            ListElement { name: qsTr("Screen");}
            ListElement { name: qsTr("Network");}
            ListElement { name: qsTr("Accounts");}
            ListElement { name: qsTr("Devices");}
            ListElement { name: qsTr("Apps");}
            ListElement { name: qsTr("About");}
        }

        GridView {
            id: tabView
            boundsBehavior: Flickable.StopAtBounds
            Layout.fillHeight: true
            Layout.preferredWidth: 256 * scaleFactor
            cellWidth: 256 * scaleFactor
            cellHeight: 80 * scaleFactor + globalSpacing
            model: tabModel
            highlight: Rectangle {
                z: 20000
                color: "#000000ff"
                border.color: "#00b3ff"
                border.width: 4 * scaleFactor
            }
            activeFocusOnTab: true
            focus: true

            delegate: SettingsDelegate {
                height: 80 * scaleFactor
                width: parent.width
                text: model.name

                onClicked: {
                    tabView.currentIndex = index
                }
            }
        }

        StackLayout {
            id: stackLayout
            Layout.fillHeight: true
            currentIndex: tabView.currentIndex
            Item {
                id: screenTab
                    ListModel {
                        id: visualModel
                        ListElement { name: qsTr("Light Theme"); action: "setTheme"; value: false }
                        ListElement { name: qsTr("Dark Theme"); action: "setTheme"; value: true }
                        ListElement { name: qsTr("Screen resolution"); action: "setResolution" }
                    }

                    GridView {
                        id: screenView
                        anchors.fill: parent
                        cellHeight: 80 * scaleFactor + globalSpacing
                        cellWidth: width
                        model: visualModel
                        highlight: Rectangle {
                            z: 20000
                            color: "#000000ff"
                            border.color: "#00b3ff"
                            border.width: 4 * scaleFactor
                        }
                        activeFocusOnTab: true
                        focus: true

                        delegate: SettingsDelegate {
                            height: 80 * scaleFactor
                            width: parent.width
                            background: Rectangle {
                                color: Theme.tabbar
                                border.color: Theme.button
                                border.width: 4 * scaleFactor
                            }
                            text: model.action === "setResolution"
                                  ? qsTr("Screen resolution: ") + Screen.width + "x" + Screen.height
                                  : model.name


                            onClicked: {
                                screenView.currentIndex = index;
                                switch(model.action){
                                case "setTheme":
                                    Theme.setTheme(model.value)
                                    break;
                                default:
                                    break;
                                }
                            }
                        }
                    }
            }
            Item {
                id: networkTab
            }
            Item {
                id: accountsTab
            }
            Item {
                id: devicesTab
            }
            Item {
                id: appsTab
            }
            Item {
                id: aboutTab


                ListModel {
                    id: aboutModel
                    ListElement { name: qsTr("System version: "); action: "getVersion";}
                }

                GridView {
                    id: aboutView
                    anchors.fill: parent
                    cellHeight: 80 * scaleFactor + globalSpacing
                    cellWidth: width
                    model: aboutModel
                    highlight: Rectangle {
                        z: 20000
                        color: "#000000ff"
                        border.color: "#00b3ff"
                        border.width: 4 * scaleFactor
                    }
                    activeFocusOnTab: true
                    focus: true

                    delegate: SettingsDelegate {
                        height: 80 * scaleFactor
                        width: parent.width
                        background: Rectangle {
                            color: Theme.tabbar
                            border.color: Theme.button
                            border.width: 4 * scaleFactor
                        }
                        text: model.action === "getVersion"
                              ? qsTr("System version: ") +  SystemInformation.kernelType + " " + SystemInformation.kernelVersion + " " + SystemInformation.currentCpuArchitecture + " " + SystemInformation.prettyProductName
                              : model.name



                        onClicked: {
                            aboutView.currentIndex = index;
                            switch(model.action){
                            default:
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    Rectangle {
        width: parent.width
        height: 128 * scaleFactor

        gradient: Gradient {
            GradientStop {
                position: 0.629808
                color: Theme.background
            }
            GradientStop {
                position: 1.0
                color: "transparent"
            }
        }
    }


}
