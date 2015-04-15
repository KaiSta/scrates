import QtQuick 2.0

Rectangle {
    SystemPalette { id: palette }
    id: bottomBar
    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
    }
    height: buttonRow.height * 1.2
    color: Qt.darker(palette.window, 1.1)

    ButtonBar { }
}
