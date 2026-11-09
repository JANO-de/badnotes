import QtQuick
import Inkwell

// Step 2 placeholder: paged notebook (page list, templates, zoom/pan).
Item {
    property alias canvas: ink
    Rectangle {
        anchors.fill: parent; anchors.margins: 24; color: Theme.paper; radius: 6
        InkCanvas { id: ink; anchors.fill: parent }
    }
}
