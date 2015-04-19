import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.0

Item {
    clip: true

    property alias columnHeight: columnLayout.height

    // Message Dialog: Deleting Provider
    MessageDialog {
        id: removeProviderMsgDialog
        modality: Qt.WindowModal
        title: qsTr("Removing selected provider")
        text: qsTr("Removing provider")
        informativeText: "The selected provider will be removed and deleted permanently in the provider.xml file. Are you sure?"
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: removeProvider()
    }

    ColumnLayout {
        id: columnLayout
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 10
        }
        spacing: 5

        Label {
            text: qsTr("Providers:")
        }
        Text {
            text: qsTr("(Info) Your encrypted containers will be stored under the selected provider location.")
            font.italic: true
            font.pixelSize: 10
        }

        TableView {
            id: providersTable
            model: providersModel
            Layout.fillWidth: true
            TableViewColumn {
                title: qsTr("Placeholder")
                role: "placeholder"
            }
            TableViewColumn {
                title: qsTr("Location")
                role: "location"
            }
        }

        RowLayout {
            Button {
                // TODO
                text: qsTr("Refresh")
                visible: false
                onClicked: refreshProviders()
            }

            Button {
                text: qsTr("Remove")
                enabled: (providersTable.currentRow >= 0 ? true : false)
                onClicked: removeProviderMsgDialog.open()
            }
        }

        Label {
            text: qsTr("Add new provider:")
        }

        TextField {
            id: placeholderText
            placeholderText: qsTr("Placeholder, e.g. $Dropbox, $MyDir01")
            Layout.fillWidth: true
            validator: RegExpValidator {
                regExp: /[$]?[a-zA-Z]+[0-9]*/
            }
            maximumLength: 32
            onTextChanged: validate()
            function validate() {
                if (this.text.length === 1 && this.text[0] !== "$")
                    this.text = "$" + this.text;
            }
        }
        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: locationText
                placeholderText: qsTr("Location")
                Layout.fillWidth: true
            }
            Button {
                text: qsTr("Choose...")
            }
        }
        Button {
            text: qsTr("Add")
            enabled: validate()
            anchors.right: parent.right
            onClicked: addProvider()
        }
    }

    function validate() {
        var regex = /[$][a-zA-Z]+[0-9]*/;
        return regex.test(placeholderText.text) && locationText.text.length && !_containerModel.containsProvider(placeholderText.text);
    }

    function addProvider() {
        _containerModel.addProvider(placeholderText.text, locationText.text);
        providersModel.reload();
        placeholderText.text = "";
        locationText.text = "";
        providersTable.forceActiveFocus();
    }

    function removeProvider() {
        _containerModel.deleteProvider(providersModel.get(providersTable.currentRow).placeholder);
        providersModel.reload();
    }

    function refreshProviders() {
        _containerModel.refreshProviderList();
        providersModel.reload();
    }

    function save() {
        // Nothing to save so far. Maybe someday.
    }
}
