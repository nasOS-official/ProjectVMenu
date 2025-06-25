import QtQuick 2.12
import QtQuick.Controls 2.12
import "themes"

Item {
    id: root
    property alias size: root.width
    property int iconSize: 30
    property alias iconSource: button.icon.source
    signal clicked

    height: width
    RoundButton {
        id: button
        anchors.fill: parent
        icon.width: iconSize
        icon.height: iconSize
        icon.color: Theme.text
        palette.button: Theme.button

        onClicked: {
            root.clicked();
        }
    }
}
