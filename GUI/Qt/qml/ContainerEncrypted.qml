import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.1

Item {
    ColumnLayout {
        anchors.centerIn : parent
        Label {
            text: qsTr("The selected container is encrypted.\nPlease enter the password to decrypt the container.")
            horizontalAlignment: Text.AlignHCenter
        }

        TextField {
            anchors.left: parent.left
            anchors.right: parent.right
            placeholderText: qsTr("Password")
            echoMode: TextInput.Password
            onAccepted: console.log("TODO: decrypt container")
        }

        Button {
            text: qsTr("Decrypt and mount container")
            onClicked: console.log("TODO: decrypt & mount container")
        }
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
