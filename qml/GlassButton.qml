import QtQuick
import Inkwell

Rectangle {
    id: b
    property string label
    property bool active: false
    signal clicked
    implicitWidth: Math.max(40, t.implicitWidth + 20)
    implicitHeight: 40
    radius: 14
    opacity: enabled ? 1 : 0.4
    color: active ? Theme.accent : (ma.pressed ? Theme.glassEdge : "transparent")
    Text { id: t; anchors.centerIn: parent; text: b.label; font.pixelSize: 13
           color: b.active ? "white" : Theme.text }
    MouseArea { id: ma; anchors.fill: parent; onClicked: b.clicked() }
}
