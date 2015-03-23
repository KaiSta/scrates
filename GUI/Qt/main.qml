import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1

ApplicationWindow {
    id: root
    title: qsTr("tempest")
    width: 640
    minimumWidth: 640
    height: 480
    minimumHeight: 480
    visible: true

    property alias containerList: containerTable

    FileDialog {
        id: fileDialog
        folder: "file:///Users"
        selectFolder: true
        onAccepted: messageDialog.show(fileUrl);
    }

    Action {
        id: openAction
        text: "Open Container"
        onTriggered: fileDialog.open()
        shortcut: "Ctrl+O"
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&Open Container")
                action: openAction
            }
            MenuItem {
                text: qsTr("&New Container")
                onTriggered: messageDialog.show(qsTr("Open action triggered"));
            }
            MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit();
            }
        }
    }

    toolBar: ToolBar {
        RowLayout {
            width: parent.width
            anchors.fill: parent

            Button {
                text: qsTr("Add")
                onClicked: {
                    _containerModel.addContainer("asdsa", "asdas")
                    viewLoader.source = "ContainerNew.qml"
                }
            }

            Button {
                text: qsTr("Remove")
                onClicked: removeContainerMsgDialog.open()
                enabled: (containerList.currentRow > -1 & containerList.currentRow < containerList.rowCount ? true : false)
                // Damn it! After removing the last row (of containerList) no item is selected.
            }

            Button {
                text: qsTr("Refresh")
                // Todo
            }

            Item { Layout.fillWidth: true }

            Button {
                text: qsTr("Sync")
                // Todo
            }
        }
    }

    // Main Window
    SplitView {
        anchors.fill: parent

        // Displays the local stored container files
        TableView {






            id: containerTable
            model: _containerModel

            frameVisible: false
            Layout.minimumWidth: 100
            Layout.maximumWidth: 300

            onActivated: {
                console.log("activated") // WTF
                console.log(containerList.currentRow)
                viewLoader.source = "ContainerEncrypted.qml"
            }

            TableViewColumn {
                title: qsTr("Container")
                role: "name"
            }






        }

        Loader {
            id: viewLoader
            source: "Welcome.qml"
        }
    }

    MessageDialog {
        id: messageDialog
        title: qsTr("May I have your attention, please?")

        function show(caption) {
            messageDialog.text = caption;
            messageDialog.open();
        }
    }

    // Message Dialog: Deleting Container
    MessageDialog {
        id: removeContainerMsgDialog
        modality: Qt.WindowModal
        title: "Removing selected container"
        text: "Removing " + "'" + _containerModel.get(containerList.currentRow, 2) + "'" + " container"
        informativeText: "The selected container will be removed permanently from your hard drive. Are you sure?"
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Yes | StandardButton.Abort
        onYes: _containerModel.removeContainer(containerList.currentRow)
    }
}
