import QtQuick
import QtQuick.Controls
import Inkwell

LibraryView {
    id: lib
    model: Library {}
    delegate: LibraryDelegate {}
    anchors.fill: parent
    spacing: 12
    padding: 12
    flipable: true
}

LibraryDelegate {
    id: delegate
    property var libraryModel: Library {}
    width: 180; height: 240
    radius: 12
    color: Theme.paper

    // Title text
    Text {
        id: titleText
        anchors.top: parent.top; anchors.topMargin: 8
        anchors.horizontalCenter: parent.horizontalCenter
        text: library.title; font.pixelSize: 14; color: Theme.text;
        wrapMode: Text.Wrap; maximumLineCount: 1; renderType: Text.QtRenderType::NativeRendering
    }

    // Modified indicator dot
    Rectangle {
        id: dot
        anchors.bottomRight: parent.bottomRight; anchors.bottomRightMargin: 4
        width: 8; height: 8; radius: 4
        color: library.modified && Library.model.current && Library.model.current.title === library.title ? Theme.accent : Theme.textDim
    }

    // Page count
    Text {
        id: pageCountText
        anchors.bottom: parent.bottom; anchors.bottomMargin: 8
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("%1 pages").arg(library.pageCount); font.pixelSize: 11; color: Theme.textDim
    }

    // Selected state
    Rectangle {
        anchors.fill: parent; radius: 12
        color: Library.model.current && Library.model.current.title === library.title ? Theme.accent.withAlphaF(0.15) : "transparent"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Library.model.setCurrent(delegateIndex)
            if (library.openedLibNotebook) library.openedLibNotebook(Library.model.current)
        }
    }

    // Context menu via right-click
    Menu {
        id: ctxMenu
        visible: false
        anchors.centerIn: parent

        Action {
            label: "Open"
            onTriggered: {
                Library.model.setCurrent(delegateIndex)
                if (library.openedLibNotebook) library.openedLibNotebook(Library.model.current)
                ctxMenu.visible = false
            }
        }
        Action {
            label: "Rename"
            onTriggered: {
                // Show text input dialog
                TextInput {
                    id: renameInput
                    text: library.title
                    selectAll: true
                    anchors.centerIn: parent
                    width: 180; height: 40
                    font.pixelSize: 14
                }
                // Simplified: just emit a signal, QML parent will handle the rename
                if (library.renamedLibNotebook) {
                    library.renamedLibNotebook(delegateIndex, renameInput.text)
                }
                ctxMenu.visible = false
            }
        }
        Action {
            label: "Delete"
            enabled: Library.model.notebooks.size > 1
            onTriggered: Library.model.removeNotebook(delegateIndex)
            ctxMenu.visible = false
        }
        Action {
            label: "Duplicate"
            onTriggered: {
                int idx = Library.model.addNotebook(
                    qsTr("Copy of %1").arg(Library.model.current.title),
                    Library.model.current.title
                )
                Library.model.setCurrent(idx)
                ctxMenu.visible = false
            }
        }
    }

    // Right-click handler
    Rectangle {
        anchors.fill: parent
        contextMenu: ctxMenu
        onPressed: ctxMenu.visible = !ctxMenu.visible
        onReleased: ctxMenu.visible = false
    }
}