import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3

/*
 * TODO:
 * - Hide automatically after n seconds
 * - Styling
 */

Rectangle {
    id: informationMessage
    z: 100
    visible: false
    height: closeMessage.height + 4
    Layout.fillWidth: true
    color: "lightblue"
    opacity: 1
    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
    }

    Row {
        anchors.verticalCenter: parent.verticalCenter
        spacing: 5
        Text {
            id: informationText
            font.pixelSize: 10
            anchors.leftMargin: 10
        }

        ToolButton {
            id: closeMessage
            text: "X"
            height: 16
            width: 16
            onClicked: informationMessage.close()
        }
    }
    // type: success, info, warning, error
    function show(msg, type) {
        var color = "blue";
        if (type == "success") color = "green";
        else if (type == "info") color = "yellow";
        else if (type == "warning") color = "orange";
        else if (type == "error") color = "red";

        informationMessage.color = color;
        informationText.text = msg;
        informationMessage.visible = true;
    }
    function close() {
        informationMessage.visible = false;
    }
}
