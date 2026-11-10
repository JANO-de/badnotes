import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Inkwell

// Modal prompt: optional text field + OK/Cancel.
Popup {
    id: p
    property string title
    property string message
    property bool showField: false
    property string okLabel: qsTr("OK")
    property bool danger: false
    property alias text: field.text
    signal submitted(string value)

    modal: true; focus: true
    anchors.centerIn: Overlay.overlay
    width: 340
    padding: 20
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    onOpened: if (showField) { field.forceActiveFocus(); field.selectAll() }

    background: Rectangle { radius: 22; color: Theme.paper; border.width: 1; border.color: Theme.glassEdge }
    Overlay.modal: Rectangle { color: Qt.rgba(0, 0, 0, 0.35) }

    contentItem: ColumnLayout {
        spacing: 12
        Text { text: p.title; color: Theme.text; font.pixelSize: 17; font.weight: Font.DemiBold; Layout.fillWidth: true; wrapMode: Text.Wrap }
        Text { visible: p.message !== ""; text: p.message; color: Theme.textDim; font.pixelSize: 13; Layout.fillWidth: true; wrapMode: Text.Wrap }
        TextField {
            id: field
            visible: p.showField
            Layout.fillWidth: true
            color: Theme.text; font.pixelSize: 14
            selectByMouse: true
            background: Rectangle { radius: 10; color: Theme.bg; border.width: 1; border.color: field.activeFocus ? Theme.accent : Theme.glassEdge }
            onAccepted: okBtn.clicked()
        }
        RowLayout {
            Layout.alignment: Qt.AlignRight
            GlassButton { label: qsTr("Cancel"); onClicked: p.close() }
            GlassButton {
                id: okBtn
                label: p.okLabel; active: !p.danger
                onClicked: { p.submitted(field.text); p.close() }
            }
        }
    }
}
