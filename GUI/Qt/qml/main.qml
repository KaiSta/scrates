import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import Qt.labs.settings 1.0

ApplicationWindow {
    id: root
    title: qsTr("tempest")
    width: 640
    minimumWidth: 640
    height: 480
    minimumHeight: 480
    visible: true

    Settings {
        // property alias x: root.x
        // property alias y: root.y
        property alias width: root.width
        property alias height: root.height
    }

    property alias containerList: containerTable

    onClosing: {
        // TODO:
        // - save QDir List of all Container files
    }

    FileDialog {
        id: fileDialog
        title: "TODO"
        modality: Qt.NonModal
        folder: "file:///Users"
        selectFolder: true
        onAccepted: console.log(fileUrl)
    }

    FileDialog {
        id: importContainerFileDialog
        title: "Import"
        modality: Qt.NonModal
        folder: "file:///Users"
        nameFilters: [ "Image files (*.png *.jpg)", "All files (*)" ]
        selectedNameFilter: "Image files (*.png *.jpg)"
        onAccepted: console.log(fileUrl)
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
                shortcut: "Ctrl+N"
                // onTriggered: TODO
            }
            MenuSeparator { }
            MenuItem {
                text: qsTr("&Import")
                shortcut: "Ctrl+I"
                onTriggered: importContainerFileDialog.open()
            }
            MenuItem {
                text: qsTr("&Export")
                shortcut: "Ctrl+E"
            }
            MenuSeparator { }
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

            Button /*ToolButton*/ {
                text: qsTr("+")
                tooltip: qsTr("Create a new container")
                onClicked: {
                    //test.open()
                    viewLoader.source = "ContainerNew.qml"
                }
            }

            Button {
                text: qsTr("Import")
                tooltip: qsTr("Import an existing container")
                onClicked: messageDialog.open()
            }

            Button {
                text: qsTr("Refresh")
                tooltip: qsTr("Refresh the container list")
                // Todo
            }

            Item { Layout.fillWidth: true }

            Button {
                text: qsTr("Remove")
                tooltip: qsTr("Remove the selected container")
                onClicked: removeContainerMsgDialog.open()
                enabled: (containerList.currentRow > -1 & containerList.currentRow < containerList.rowCount ? true : false)
                // Damn it! After removing the last row (of containerList) no item is selected (visually)
            }

            Button {
                text: qsTr("Show")
                tooltip: qsTr("Open container directory")
                onClicked: _containerModel.currentContainer().openDirectory()
                enabled: (containerList.currentRow > -1 & containerList.currentRow < containerList.rowCount ? true : false)
            }

            Button {
                text: qsTr("Sync")
                // TODO
            }
        }
    }


    // In progress. TODO.
    MessageBar {

    }

    // Main Window
    SplitView {
        anchors.fill: parent

        TableView {
            id: containerTable
            model: _containerModel
            frameVisible: false
            Layout.minimumWidth: 180
            Layout.maximumWidth: 300
            onModelChanged: {
                if (_containerModel)
                    _containerModel.setCurrentContainer(-1)
            }
            onClicked: {
                _containerModel.setCurrentContainer(containerList.currentRow)
                viewLoader.source = getMainView()
            }
            onActivated: {
                _containerModel.setCurrentContainer(containerList.currentRow)
                viewLoader.source = getMainView()
            }

            TableViewColumn {
                title: qsTr("Container (" + containerList.rowCount + ")")
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
        title: qsTr("Removing selected container")
        text: qsTr("Removing container")
        informativeText: "The selected container will be removed permanently from your hard drive. Are you sure?"
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            _containerModel.remove(containerList.currentRow)
            viewLoader.source = "Welcome.qml"
        }
    }

    NewContainerDialog {
        id: test
    }

    function getMainView()
    {
        var qml = "Welcome.qml"
        if (containerList.currentRow < 0)
            return qml

        var container = _containerModel.get(containerList.currentRow)

        if (container.encrypted)
            qml = "ContainerEncrypted.qml"
        else
            qml = "ContainerDecrypted.qml"

        return qml
    }
}
