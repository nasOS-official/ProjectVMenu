pragma Singleton

import QtQuick 2.12

Item {
    QtObject{
        id: themes
        readonly property var light: ["#ffffff", "#dbdbdb", "#cacaca", "#000000"]
        readonly property var dark: ["#3c3c3c", "#454545", "#535353", "#ffffff"]
    }
    property var currentTheme: themes.dark

    function setTheme(isDark) {
        currentTheme = isDark ? themes.dark : themes.light
    }


    readonly property string background: currentTheme[0]
    readonly property string tabbar: currentTheme[1]
    readonly property string button: currentTheme[2]
    readonly property string text: currentTheme[3]

}
