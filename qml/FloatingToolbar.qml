import QtQuick
import QtQuick.Layouts
import Inkwell

GlassPanel {
    id: bar
    required property InkCanvas canvas
    property string tool: "pen"
    implicitWidth: row.implicitWidth + 28
    implicitHeight: 56
    radius: 28

    component ToolBtn: Rectangle {
        property string label
        property bool active: false
        signal clicked
        implicitWidth: 44; implicitHeight: 40; radius: 14
        color: active ? Theme.accent : "transparent"
        Text { anchors.centerIn: parent; text: parent.label; font.pixelSize: 13; color: parent.active ? "white" : Theme.text }
        MouseArea { anchors.fill: parent; onClicked: parent.clicked() }
    }

    RowLayout {
        id: row; anchors.centerIn: parent; spacing: 6
        ToolBtn { label: "Pen"; active: bar.tool === "pen"; onClicked: { bar.tool = "pen"; bar.canvas.tool = "pen" } }
        ToolBtn { label: "HL"; active: bar.tool === "highlighter"; onClicked: { bar.tool = "highlighter"; bar.canvas.tool = "highlighter" } }
        ToolBtn { label: "Era"; active: bar.tool === "eraser"; onClicked: { bar.tool = "eraser"; bar.canvas.tool = "eraser" } }
        Rectangle { width: 1; height: 24; color: Theme.glassEdge }
        Repeater {
            model: ["#1c1c1e", "#0a84ff", "#ff453a", "#30d158", "#ffd60a"]
            Rectangle {
                required property string modelData
                width: 26; height: 26; radius: 13; color: modelData
                border.width: bar.canvas.penColor == modelData ? 3 : 1; border.color: Theme.accent
                MouseArea { anchors.fill: parent; onClicked: bar.canvas.penColor = parent.modelData }
            }
        }
        Rectangle { width: 1; height: 24; color: Theme.glassEdge }
        ToolBtn { label: "↶"; opacity: bar.canvas.canUndo ? 1 : 0.4; onClicked: bar.canvas.undo() }
        ToolBtn { label: "↷"; opacity: bar.canvas.canRedo ? 1 : 0.4; onClicked: bar.canvas.redo() }
    }
}
