import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3

Rectangle {
    id: informationMessage
    z: 100
    height: closeMessage.height + 10
    //preferedhight
    Layout.fillWidth: true
    color: "lightblue"
    opacity: 0.9
    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
    }

    Row {
        anchors.verticalCenter: parent.verticalCenter
        spacing: 5
        Label {
            id: informationText

            text: "error message or something like thatttt"
            anchors.leftMargin: 10
        }

        ToolButton {
            id: closeMessage
            text: "X"
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
