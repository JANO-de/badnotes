import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Inkwell

ApplicationWindow {
    id: win
    visible: true; width: 1280; height: 800
    title: (mode === "editor" ? nb.title + " — " : "") + "Inkwell"
    color: Theme.bg

    Notebook { id: nb }
    Updater { id: updater; onUpdated: win.showToast(qsTr("Update installed — restart Inkwell to apply"), Theme.accent) }
    property string mode: "library"   // library | editor
    property bool sidebarVisible: true
    Library { id: lib }

    function openPath(path) { if (path !== "" && nb.load(lib.fileUrl(path))) mode = "editor" }
    function closeNotebook() { if (nb.filePath !== "") nb.saveCurrent(); lib.refresh(); mode = "library" }
    function newNotebook() { openPath(lib.createNotebook()) }

    LibraryView {
        anchors.fill: parent
        visible: win.mode === "library"
        library: lib
        onOpenRequested: (path) => win.openPath(path)
    }

    Item {
        id: editor
        anchors.fill: parent
        visible: win.mode === "editor"

        NotebookView { id: view; anchors.fill: parent; notebook: nb }

        GlassPanel {
            source: view
            x: 16; y: 16; height: 56; radius: 28
            width: topRow.implicitWidth + 24
            Row {
                id: topRow; anchors.centerIn: parent; spacing: 4
                GlassButton { label: "‹ " + qsTr("Library"); onClicked: win.closeNotebook() }
                GlassButton { label: "☰"; active: win.sidebarVisible; onClicked: win.sidebarVisible = !win.sidebarVisible }
            }
        }
        FloatingToolbar {
            source: view; notebook: nb
            width: implicitWidth; height: implicitHeight
            x: (editor.width - width) / 2; y: 16
        }
        PageSidebar {
            visible: win.sidebarVisible
            source: view; notebook: nb; view: view
            x: 16; y: 90; width: 132; height: editor.height - 90 - 90
        }
        PageBar {
            id: pageBar
            source: view; notebook: nb; view: view
            width: implicitWidth; height: implicitHeight
            x: (editor.width - width) / 2; y: editor.height - height - 16
            onOpenRequested: openDlg.open()
            onSaveRequested: win.save()
        }
    }

    function save() { nb.filePath ? nb.saveCurrent() : saveDlg.open() }

    FileDialog {
        id: openDlg
        title: qsTr("Open notebook")
        nameFilters: ["Inkwell (*.inkwell)"]
        onAccepted: nb.load(selectedFile)
    }
    FileDialog {
        id: saveDlg
        title: qsTr("Save notebook")
        fileMode: FileDialog.SaveFile
        nameFilters: ["Inkwell (*.inkwell)"]
        defaultSuffix: "inkwell"
        onAccepted: nb.save(selectedFile)
    }

    Shortcut { enabled: win.mode === "editor"; sequences: [StandardKey.Undo]; onActivated: nb.undo() }
    Shortcut { enabled: win.mode === "editor"; sequences: [StandardKey.Redo, "Ctrl+Shift+Z"]; onActivated: nb.redo() }
    Shortcut { enabled: win.mode === "editor"; sequences: [StandardKey.Save]; onActivated: win.save() }
    Shortcut { sequences: [StandardKey.Open]; onActivated: openDlg.open() }
    Shortcut { sequences: [StandardKey.New]; onActivated: win.newNotebook() }
    Shortcut { enabled: win.mode === "editor"; sequences: ["Escape"]; onActivated: win.closeNotebook() }

    Rectangle {
        id: toast
        property alias text: toastText.text
        anchors.horizontalCenter: parent.horizontalCenter; y: 84
        width: toastText.implicitWidth + 32; height: 36; radius: 18
        property color tone: "#ff453a"
        color: tone; opacity: 0; visible: opacity > 0
        Behavior on opacity { NumberAnimation { duration: 200 } }
        Text { id: toastText; anchors.centerIn: parent; color: "white"; font.pixelSize: 13 }
        Timer { id: toastTimer; interval: 3500; onTriggered: toast.opacity = 0 }
    }
    Connections {
        target: nb
        function onError(msg) { win.showToast(msg, "#ff453a") }
    }
    function showToast(msg, tone) { toast.text = msg; toast.tone = tone; toast.opacity = 1; toastTimer.restart() }
}
