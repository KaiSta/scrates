import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.1

import "UIComponents"

Item {
    ColumnLayout {
        anchors.centerIn: parent
        width: 300
        Text {
            text: qsTr("The selected container <strong>" + _containerModel.currentContainer().name + "</strong> is encrypted. Please enter the password to decrypt the container.")
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }

        TextField {
            id: passwordText
            placeholderText: qsTr("Password")
            echoMode: TextInput.Password
            Layout.fillWidth: true
            onAccepted: mount()
            onTextChanged: closeMessageBar()
        }

        Button {
            text: qsTr("Decrypt and mount container")
            enabled: (passwordText.length ? true : false)
            onClicked: mount()
            Layout.fillWidth: true
        }
    }

    function mount() {
        if (_containerModel.currentContainer().mount(passwordText.text)) {
            updateView();
        }
        else {
            messageBar.show("Mounting failed. Wrong password?", "error");
            passwordText.selectAll();
        }
    }

    function closeMessageBar() {
        if (messageBar.visible) messageBar.close();
    }

    /* MessageDialog (ERROR)
     * why: Error while mounting/encrypting container
     * example text: press yes if you want to sync the container which will probably remove conflicting files or no if you want to repair it manually and retry to open the container. */
    MessageDialog {
        id: messageDialog
        title: "TITLE"
        text: "ERROR MESSAGE"
        informativeText: "Pastry ice cream sesame snaps cheesecake sugar plum ice cream jelly. Pastry bonbon icing donut sweet roll chocolate."
        detailedText: "Donut tiramisu macaroon gingerbread halvah tart biscuit. Chocolate chupa chups caramels pudding gingerbread."
        icon: StandardIcon.Critical
        onButtonClicked: console.log("clicked button " + clickedButton)
        standardButtons: StandardButton.Yes | StandardButton.No
    }
}
