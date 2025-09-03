pragma Singleton

import QtQuick 2.12

Item {
    QtObject{
        id: themes
        readonly property list<string> light: ["#ffffff", "#dbdbdb", "#cacaca", "#000000", "#d5d5d5"]
        readonly property list<string> dark: ["#3c3c3c", "#454545", "#535353", "#ffffff", "#262626"]
    }
    property list<string> currentTheme: themes.dark
    property bool isDarkTheme: true

    function setTheme(isDark) {
        isDarkTheme = isDark;
        currentTheme = isDark ? themes.dark : themes.light;
    }

    readonly property string background: currentTheme[0]
    readonly property string tabbar: currentTheme[1]
    readonly property string button: currentTheme[2]
    readonly property string buttonHovered: isDarkTheme ? Qt.lighter(button, 1.1) : Qt.darker(button, 1.1)
    readonly property string buttonPressed: isDarkTheme ? Qt.lighter(button, 1.2) : Qt.darker(button, 1.2)
    readonly property string text: currentTheme[3]
    readonly property string backgroundAlt: currentTheme[4]
}
