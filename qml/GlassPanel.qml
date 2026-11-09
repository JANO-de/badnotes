import QtQuick
import QtQuick.Effects
import Inkwell

// In-app glass: blurs the `source` item region behind this panel (compositor independent).
Item {
    id: root
    property Item source
    property real radius: Theme.radius
    default property alias content: inner.data

    Rectangle {
        id: base; anchors.fill: parent; radius: root.radius; color: Theme.paper
        layer.enabled: true
        layer.effect: MultiEffect { shadowEnabled: true; shadowColor: Theme.shadow; shadowBlur: 1.0; shadowVerticalOffset: 6 }
    }
    ShaderEffectSource {
        id: snap; anchors.fill: parent; visible: false; live: true
        sourceItem: root.source
        sourceRect: root.source ? Qt.rect(root.x, root.y, root.width, root.height) : Qt.rect(0, 0, 0, 0)
    }
    MultiEffect {
        anchors.fill: parent; source: snap
        autoPaddingEnabled: false
        blurEnabled: Theme.glassEnabled; blur: Theme.blur; blurMax: 48; saturation: 0.4
        maskEnabled: true; maskSource: mask
    }
    Rectangle { id: mask; anchors.fill: parent; radius: root.radius; visible: false; layer.enabled: true }
    Rectangle { anchors.fill: parent; radius: root.radius; color: Theme.glass; border.width: 1; border.color: Theme.glassEdge }
    Item { id: inner; anchors.fill: parent }
}
