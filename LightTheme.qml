// pragma Singleton

import QtQuick 2.12

QtObject {
 property color  foregroundColor: "#2e2920"
 property color  textFieldDisabledForegroundColor: foregroundColor
}
// Item{
//     QtObject{
//         id: themes
//         readonly property var light: ["#F3C892", "#FFF1BD", "#A3DA8D", "#146356"]
//         readonly property var dark: ["#040303", "#461111", "#A13333", "#B3541E"]
//     }
//     property var currentTheme: themes.light

//     readonly property string primary: currentTheme[0]
//     readonly property string secondary: currentTheme[1]
//     readonly property string tertiary: currentTheme[2]
//     readonly property string quaternary: currentTheme[3]
// }
