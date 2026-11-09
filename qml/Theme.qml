pragma Singleton
import QtQuick

QtObject {
    property string mode: "system" // system | light | dark
    readonly property bool dark: mode === "dark" || (mode === "system" && Qt.styleHints.colorScheme === Qt.Dark)
    property color accent: "#0a84ff"
    readonly property color bg:        dark ? "#1c1c1e" : "#f2f2f7"
    readonly property color paper:     dark ? "#2c2c2e" : "#ffffff"
    readonly property color text:      dark ? "#f5f5f7" : "#1c1c1e"
    readonly property color textDim:   dark ? "#a1a1a6" : "#6e6e73"
    readonly property color glass:     dark ? Qt.rgba(0.17,0.17,0.19,0.55) : Qt.rgba(1,1,1,0.55)
    readonly property color glassEdge: dark ? Qt.rgba(1,1,1,0.14) : Qt.rgba(1,1,1,0.85)
    readonly property color shadow:    dark ? Qt.rgba(0,0,0,0.5) : Qt.rgba(0,0,0,0.18)
    // paper stays light in dark mode (ink is dark)
    readonly property color sheet:     dark ? "#f4f4f1" : "#ffffff"
    readonly property color sheetLine: "#cfd6e4"
    property real radius: 22
    property real blur: 1.0
    property bool glassEnabled: true
}
