import QtQuick
import QtQuick.Layouts
import Inkwell

GlassPanel {
    id: bar
    required property Notebook notebook
    implicitWidth: row.implicitWidth + 28
    implicitHeight: 56
    radius: 28

    RowLayout {
        id: row; anchors.centerIn: parent; spacing: 6
        GlassButton { label: "Pen"; active: Tools.tool === "pen"; onClicked: Tools.tool = "pen" }
        GlassButton { label: "HL"; active: Tools.tool === "highlighter"; onClicked: Tools.tool = "highlighter" }
        GlassButton { label: "Era"; active: Tools.tool === "eraser"; onClicked: Tools.tool = "eraser" }
        Rectangle { width: 1; height: 24; color: Theme.glassEdge }
        Repeater {
            model: ["#1c1c1e", "#0a84ff", "#ff453a", "#30d158", "#ffd60a"]
            Rectangle {
                required property string modelData
                width: 26; height: 26; radius: 13; color: modelData
                border.width: Tools.penColor == modelData ? 3 : 1; border.color: Theme.accent
                MouseArea { anchors.fill: parent; onClicked: Tools.penColor = parent.modelData }
            }
        }
        Rectangle { width: 1; height: 24; color: Theme.glassEdge }
        GlassButton { label: "↶"; enabled: bar.notebook.canUndo; onClicked: bar.notebook.undo() }
        GlassButton { label: "↷"; enabled: bar.notebook.canRedo; onClicked: bar.notebook.redo() }
    }
}
