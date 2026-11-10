import QtQuick
import Inkwell

// Vertical page scroller with zoom. Touch = pan/pinch, pen = draw (pen near => touch ignored).
Item {
    id: root
    required property Notebook notebook
    property real zoom: 1.0
    readonly property real pageW: 794
    readonly property real pageH: 1123
    readonly property real gap: 28
    readonly property real topMargin: 90
    readonly property real bottomMargin: 110
    readonly property real unit: (pageH + gap) * zoom
    readonly property real minZoom: 0.25
    readonly property real maxZoom: 4
    readonly property int currentPage: Math.max(0, Math.min(notebook.pageCount - 1,
        Math.floor((flick.contentY + flick.height / 2 - topMargin) / unit)))

    function cw(z) { return Math.max(flick.width, pageW * z + 48) }
    function clampZoom(z) { return Math.max(minZoom, Math.min(maxZoom, z)) }

    // Keep the content point (in zoom-independent units) under the viewport point (cx, cy).
    property real anchorU: 0
    property real anchorV: 0
    function anchorAt(cx, cy) {
        anchorU = (flick.contentX + cx - cw(zoom) / 2) / zoom
        anchorV = (flick.contentY + cy - topMargin) / zoom
    }
    function applyZoom(z, cx, cy) {
        z = clampZoom(z)
        zoom = z
        flick.contentX = Math.max(0, anchorU * z + cw(z) / 2 - cx)
        flick.contentY = Math.max(0, anchorV * z + topMargin - cy)
        flick.returnToBounds()
    }
    function zoomBy(f, cx = flick.width / 2, cy = flick.height / 2) { anchorAt(cx, cy); applyZoom(zoom * f, cx, cy) }
    function fitWidth() { anchorAt(flick.width / 2, flick.height / 2); applyZoom((flick.width - 48) / pageW, flick.width / 2, flick.height / 2) }
    function scrollToPage(i) { flick.contentY = Math.max(0, topMargin + i * unit - 60) }

    Connections { target: root.notebook; function onFileChanged() { flick.contentX = 0; flick.contentY = 0 } }

    Flickable {
        id: flick
        anchors.fill: parent
        clip: true
        interactive: !Tools.penNear && !pinch.active
        contentWidth: root.cw(root.zoom)
        contentHeight: root.topMargin + root.notebook.pageCount * root.unit - root.gap * root.zoom + root.bottomMargin

        Column {
            id: col
            y: root.topMargin
            x: (flick.contentWidth - width) / 2
            width: root.pageW * root.zoom
            spacing: root.gap * root.zoom
            Repeater {
                model: root.notebook
                delegate: Item {
                    id: d
                    required property var page
                    width: root.pageW * root.zoom
                    height: root.pageH * root.zoom
                    // only instantiate canvases near the viewport (step 04 replaces this with tile cache)
                    readonly property bool near: y + col.y + height > flick.contentY - 600
                                              && y + col.y < flick.contentY + flick.height + 600
                    Rectangle { anchors.fill: parent; color: Theme.sheet; border.width: 1; border.color: Theme.glassEdge }
                    Loader {
                        anchors.fill: parent
                        active: d.near
                        sourceComponent: InkCanvas {
                            page: d.page
                            paperColor: Theme.sheet
                            lineColor: Theme.sheetLine
                        }
                    }
                }
            }
        }

        PinchHandler {
            id: pinch
            target: null
            acceptedDevices: PointerDevice.TouchScreen
            minimumPointCount: 2
            maximumPointCount: 2
            property real startZoom: 1
            onActiveChanged: if (active) { startZoom = root.zoom; root.anchorAt(centroid.position.x, centroid.position.y) }
            onActiveScaleChanged: if (active) root.applyZoom(startZoom * activeScale, centroid.position.x, centroid.position.y)
            onCentroidChanged: if (active) root.applyZoom(root.zoom, centroid.position.x, centroid.position.y)
        }
        WheelHandler {
            acceptedModifiers: Qt.ControlModifier
            onWheel: (ev) => root.zoomBy(Math.pow(1.0015, ev.angleDelta.y), point.position.x, point.position.y)
        }
    }
}
