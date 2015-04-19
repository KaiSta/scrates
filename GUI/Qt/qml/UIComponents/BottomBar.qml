import QtQuick 2.4

Rectangle {
    SystemPalette { id: palette }
    id: bottomBar
    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
    }
    color: Qt.darker(palette.window, 1.1)

    ButtonBar { }
}
