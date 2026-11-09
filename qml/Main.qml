import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Inkwell

ApplicationWindow {
    id: win
    visible: true; width: 1280; height: 800
    title: nb.title + " — Inkwell"
    color: Theme.bg

    Notebook { id: nb }
    Updater { id: updater; onUpdated: win.showToast(qsTr("Update installed — restart Inkwell to apply"), Theme.accent) }
    NotebookView { id: view; anchors.fill: parent; notebook: nb }

    FloatingToolbar {
        source: view; notebook: nb
        width: implicitWidth; height: implicitHeight
        x: (parent.width - width) / 2; y: 16
    }
    PageBar {
        id: pageBar
        source: view; notebook: nb; view: view
        width: implicitWidth; height: implicitHeight
        x: (parent.width - width) / 2; y: parent.height - height - 16
        onOpenRequested: openDlg.open()
        onSaveRequested: win.save()
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

    Shortcut { sequences: [StandardKey.Undo]; onActivated: nb.undo() }
    Shortcut { sequences: [StandardKey.Redo, "Ctrl+Shift+Z"]; onActivated: nb.redo() }
    Shortcut { sequences: [StandardKey.Save]; onActivated: win.save() }
    Shortcut { sequences: [StandardKey.Open]; onActivated: openDlg.open() }
    Shortcut { sequences: [StandardKey.New]; onActivated: nb.newNotebook() }

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
