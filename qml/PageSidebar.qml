import QtQuick
import Inkwell

// Page thumbnails: tap = go to page, long-press then drag = reorder.
GlassPanel {
    id: sb
    required property Notebook notebook
    required property NotebookView view
    property bool reordering: false
    radius: 22

    ListView {
        id: list
        anchors.fill: parent; anchors.margins: 10
        clip: true; spacing: 12
        model: sb.notebook
        interactive: !sb.reordering
        readonly property real thumbH: width * 1123 / 794
        readonly property real rowH: thumbH + spacing

        Connections {
            target: sb.view
            function onCurrentPageChanged() { if (!sb.reordering) list.positionViewAtIndex(sb.view.currentPage, ListView.Contain) }
        }

        delegate: Item {
            id: row
            required property var page
            required property int index
            readonly property bool current: index === sb.view.currentPage
            width: list.width; height: list.thumbH
            z: card.dragging ? 10 : 1

            Rectangle {
                id: card
                property bool dragging: false
                width: parent.width; height: parent.height
                radius: 5; color: Theme.sheet
                opacity: dragging ? 0.85 : 1
                scale: dragging ? 1.06 : 1
                Behavior on scale { NumberAnimation { duration: 120 } }
                PageThumb {
                    anchors.fill: parent; anchors.margins: 1
                    page: row.page; paperColor: Theme.sheet; lineColor: Theme.sheetLine
                }
                Rectangle { anchors.fill: parent; radius: parent.radius; color: "transparent"
                    border.width: row.current ? 3 : 1; border.color: row.current ? Theme.accent : Theme.glassEdge }
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom; anchors.bottomMargin: 6
                    width: num.implicitWidth + 14; height: 20; radius: 10; color: Theme.glass
                    Text { id: num; anchors.centerIn: parent; text: row.index + 1; font.pixelSize: 11; color: Theme.text }
                }
                MouseArea {
                    id: ma
                    anchors.fill: parent
                    pressAndHoldInterval: 350
                    preventStealing: sb.reordering
                    drag.target: sb.reordering ? card : null
                    drag.axis: Drag.YAxis
                    onClicked: sb.view.scrollToPage(row.index)
                    onPressAndHold: { sb.reordering = true; card.dragging = true }
                    onReleased: {
                        if (!card.dragging) return
                        const target = Math.max(0, Math.min(sb.notebook.pageCount - 1,
                            Math.round((row.y + card.y) / list.rowH)))
                        card.y = 0; card.dragging = false; sb.reordering = false
                        if (target !== row.index) sb.notebook.movePage(row.index, target)
                    }
                    onCanceled: { card.y = 0; card.dragging = false; sb.reordering = false }
                }
            }
        }
    }
}
