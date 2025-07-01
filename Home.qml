import QtQuick

Item {

    Row {
        spacing: 10
        anchors.centerIn: parent
        anchors.horizontalCenter: parent.horizontalCenter
        Repeater{
            model: 6
            Rectangle {
               width: 100
               height: 100
               color: "white"
            }
        }
    }
    Dialog{
        onClicked: { this.destroy() }
        text: "Hello!"
    }
}
