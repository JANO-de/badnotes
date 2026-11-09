import QtQuick
import QtQuick.Controls
import Inkwell

ApplicationWindow {
    visible: true; width: 1280; height: 800; title: "Inkwell"
    color: Theme.bg

    NotebookView { id: nb; anchors.fill: parent }
    FloatingToolbar {
        source: nb; canvas: nb.canvas
        width: implicitWidth; height: implicitHeight
        x: (parent.width - width) / 2; y: 16
    }
    PageSidebar { source: nb; x: 16; y: 90; width: 120; height: parent.height - 180 }
}
