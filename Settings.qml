import QtQuick
import "themes"
import QtQuick.Controls
Item {
    Button {
        id: button
        x: 15
        y: 17
        text: qsTr("Light Theme")
        onClicked: Theme.setTheme(false)
    }

    Button {
        id: button1
        x: 15
        y: 63
        text: qsTr("Dark Theme")
        onClicked: Theme.setTheme(true)
    }

}
