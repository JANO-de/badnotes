import QtQuick
import QtQuick.Layouts
import Inkwell

GlassPanel {
    id: bar
    required property Notebook notebook
    required property NotebookView view
    signal openRequested
    signal saveRequested

    readonly property var templates: ["blank", "lined", "grid", "dotted"]
    readonly property int cur: view.currentPage
    readonly property string curTemplate: notebook.pageAt(cur) ? notebook.pageAt(cur).pageTemplate : ""

    implicitWidth: row.implicitWidth + 28
    implicitHeight: 56
    radius: 28

    RowLayout {
        id: row; anchors.centerIn: parent; spacing: 4
        GlassButton { label: "−"; onClicked: bar.view.zoomBy(1 / 1.25) }
        GlassButton { label: Math.round(bar.view.zoom * 100) + "%"; onClicked: bar.view.fitWidth() }
        GlassButton { label: "+"; onClicked: bar.view.zoomBy(1.25) }
        Rectangle { width: 1; height: 24; color: Theme.glassEdge }
        GlassButton { label: "‹"; enabled: bar.cur > 0; onClicked: bar.view.scrollToPage(bar.cur - 1) }
        Text { text: (bar.cur + 1) + " / " + bar.notebook.pageCount; color: Theme.text; font.pixelSize: 13
               Layout.preferredWidth: 52; horizontalAlignment: Text.AlignHCenter }
        GlassButton { label: "›"; enabled: bar.cur < bar.notebook.pageCount - 1; onClicked: bar.view.scrollToPage(bar.cur + 1) }
        Rectangle { width: 1; height: 24; color: Theme.glassEdge }
        GlassButton { label: "+ Page"; onClicked: bar.view.scrollToPage(bar.notebook.addPage(bar.cur)) }
        GlassButton { label: "Delete"; onClicked: bar.notebook.removePage(bar.cur) }
        GlassButton { label: bar.curTemplate
            onClicked: {
                const i = bar.templates.indexOf(bar.curTemplate)
                bar.notebook.pageAt(bar.cur).pageTemplate = bar.templates[(i + 1) % bar.templates.length]
            } }
        GlassButton { label: "↑"; enabled: bar.cur > 0
            onClicked: { bar.notebook.movePage(bar.cur, bar.cur - 1); bar.view.scrollToPage(bar.cur - 1) } }
        GlassButton { label: "↓"; enabled: bar.cur < bar.notebook.pageCount - 1
            onClicked: { bar.notebook.movePage(bar.cur, bar.cur + 1); bar.view.scrollToPage(bar.cur + 1) } }
        Rectangle { width: 1; height: 24; color: Theme.glassEdge }
        GlassButton { label: "Open"; onClicked: bar.openRequested() }
        GlassButton { label: "Save"; onClicked: bar.saveRequested() }
    }
}
