import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Inkwell

Item {
    id: root
    required property Library library
    signal openRequested(string path)
    property var target: ({ name: "", path: "", isFolder: false, count: 0 })

    function showMenu(cell) {
        target = { name: cell.name, path: cell.path, isFolder: cell.isFolder, count: cell.itemCount }
        menu.popup()
    }

    Rectangle { anchors.fill: parent; color: Theme.bg }

    GridView {
        id: grid
        anchors.fill: parent
        clip: true
        topMargin: 96; bottomMargin: 24; leftMargin: 24; rightMargin: 24
        cellWidth: 210; cellHeight: 306
        model: root.library

        Text {
            visible: grid.count === 0
            anchors.centerIn: parent
            text: root.library.query !== "" ? qsTr("No notebooks match your search") : qsTr("Nothing here yet — create a notebook")
            color: Theme.textDim; font.pixelSize: 15
        }

        delegate: Item {
            id: cell
            required property string name
            required property string path
            required property bool isFolder
            required property string modifiedText
            required property double modifiedMs
            required property int itemCount
            required property string location
            width: grid.cellWidth; height: grid.cellHeight

            Rectangle {
                id: cover
                x: 20; y: 6; width: 170; height: 240; radius: 6
                color: cell.isFolder ? Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.16) : Theme.sheet
                Image {
                    anchors.fill: parent
                    visible: !cell.isFolder
                    asynchronous: true; cache: false
                    fillMode: Image.PreserveAspectCrop
                    sourceSize: Qt.size(340, 481)
                    source: cell.isFolder ? "" : "image://cover/" + encodeURIComponent(cell.path) + "?" + cell.modifiedMs
                }
                // folder glyph
                Item {
                    visible: cell.isFolder
                    anchors.centerIn: parent; width: 110; height: 86
                    Rectangle { x: 0; y: 0; width: 46; height: 20; radius: 6; color: Theme.accent; opacity: 0.8 }
                    Rectangle { x: 0; y: 12; width: 110; height: 74; radius: 10; color: Theme.accent; opacity: 0.9
                        Text { anchors.centerIn: parent; text: cell.itemCount; color: "white"; font.pixelSize: 26; font.weight: Font.DemiBold } }
                }
                Rectangle { anchors.fill: parent; radius: parent.radius; color: "transparent"; border.width: 1; border.color: Theme.glassEdge }
            }
            Text {
                x: 20; y: 252; width: 170
                text: cell.name; elide: Text.ElideRight
                color: Theme.text; font.pixelSize: 14; font.weight: Font.Medium
            }
            Text {
                x: 20; y: 272; width: 170
                text: cell.location !== "" ? cell.location : (cell.isFolder ? qsTr("%n item(s)", "", cell.itemCount) : cell.modifiedText)
                elide: Text.ElideRight
                color: Theme.textDim; font.pixelSize: 12
            }
            TapHandler {
                onTapped: cell.isFolder ? root.library.enter(cell.path) : root.openRequested(cell.path)
                onLongPressed: root.showMenu(cell)
            }
            TapHandler { acceptedButtons: Qt.RightButton; onTapped: root.showMenu(cell) }
        }
    }

    GlassPanel {
        source: grid
        x: 16; y: 16; width: parent.width - 32; height: 56; radius: 28
        RowLayout {
            anchors.fill: parent; anchors.leftMargin: 14; anchors.rightMargin: 14; spacing: 6
            GlassButton { visible: root.library.canGoUp && root.library.query === ""; label: "‹"; onClicked: root.library.up() }
            Text { text: root.library.query !== "" ? qsTr("Search") : root.library.breadcrumb
                   color: Theme.text; font.pixelSize: 16; font.weight: Font.DemiBold
                   elide: Text.ElideRight; Layout.fillWidth: true }
            TextField {
                id: search
                Layout.preferredWidth: 220; Layout.preferredHeight: 36
                placeholderText: qsTr("Search notebooks")
                color: Theme.text; font.pixelSize: 14; selectByMouse: true
                onTextChanged: root.library.query = text
                background: Rectangle { radius: 18; color: Theme.bg; opacity: 0.7; border.width: 1; border.color: search.activeFocus ? Theme.accent : Theme.glassEdge }
            }
            GlassButton { label: qsTr("+ Folder"); onClicked: { folderDlg.text = ""; folderDlg.open() } }
            GlassButton { label: qsTr("+ Notebook"); active: true; onClicked: root.openRequested(root.library.createNotebook()) }
        }
    }

    Menu {
        id: menu
        MenuItem { text: qsTr("Rename"); onTriggered: { renameDlg.text = root.target.name; renameDlg.open() } }
        MenuItem { text: qsTr("Duplicate"); enabled: !root.target.isFolder; onTriggered: root.library.duplicate(root.target.path) }
        MenuItem { text: qsTr("Delete"); onTriggered: deleteDlg.open() }
    }

    AskPopup { id: folderDlg; title: qsTr("New folder"); showField: true; okLabel: qsTr("Create")
        onSubmitted: (v) => root.library.createFolder(v) }
    AskPopup { id: renameDlg; title: qsTr("Rename"); showField: true; okLabel: qsTr("Rename")
        onSubmitted: (v) => root.library.rename(root.target.path, v) }
    AskPopup { id: deleteDlg
        title: qsTr("Delete “%1”?").arg(root.target.name)
        message: root.target.isFolder ? qsTr("The folder and everything in it will be permanently deleted.") : qsTr("This notebook will be permanently deleted.")
        okLabel: qsTr("Delete"); danger: true
        onSubmitted: root.library.remove(root.target.path) }

    Connections { target: root.library; function onError(m) { root.errorText = m; errTimer.restart() } }
    property string errorText: ""
    Rectangle {
        visible: root.errorText !== ""
        anchors.horizontalCenter: parent.horizontalCenter; y: 84
        width: errT.implicitWidth + 32; height: 36; radius: 18; color: "#ff453a"
        Text { id: errT; anchors.centerIn: parent; text: root.errorText; color: "white"; font.pixelSize: 13 }
        Timer { id: errTimer; interval: 3500; onTriggered: root.errorText = "" }
    }
}
