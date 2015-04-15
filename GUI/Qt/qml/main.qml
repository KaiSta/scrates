import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.XmlListModel 2.0

import "UIComponents"
import "Dialogs"

ApplicationWindow {
    id: root
    title: "tempest"
    width: _settings.value("ApplicationWindow/width", 640)
    minimumWidth: 640
    height: _settings.value("ApplicationWindow/height", 480)
    minimumHeight: 480
    visible: true

    property variant settingsWindow;  // settings window reference
    property alias containerList: containerTable

    onClosing: {
        // TODO: forced sync for all encrypted/mounted containers
        _settings.setValue("ApplicationWindow/width", width)
        _settings.setValue("ApplicationWindow/height", height)
    }

    XmlListModel {
        id: providersModel
        source: "file://" + _applicationDirPath + "/providers.xml"
        query: "/providers/provider"
        XmlRole {name: "placeholder"; query: "@placeholder/string()"}
        XmlRole {name: "placeholderName"; query: "substring-after(@placeholder/string(), '$')"}
        XmlRole {name: "location"; query: "@location/string()"}
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

    SettingsDialog {
        id: settingsDialog
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
                onTriggered: viewLoader.source = "ContainerNew.qml"
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
        Menu {
            title: qsTr("&Tools")
            MenuItem {
                text: qsTr("&Preferences")
                onTriggered: {
                    var component = Qt.createComponent("Dialogs/SettingsDialog.qml");
                    settingsWindow = component.createObject(root);
                    settingsWindow.show();
                }
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
                    viewLoader.source = "ContainerNew.qml"
                }
            }
            Button {
                text: qsTr(" - ")
                tooltip: qsTr("Remove the selected container")
                onClicked: removeContainerMsgDialog.open()
                enabled: (containerList.currentRow > -1 & containerList.currentRow < containerList.rowCount ? true : false)
                // Damn it! After removing the last row (of containerList) no item is selected (visually)
            }
            Button {
                text: qsTr("Import")
                tooltip: qsTr("Import an existing container")
                onClicked: messageDialog.open()
            }
            Item { Layout.fillWidth: true }
        }
    }

    // In progress. TODO.
    MessageBar {

    }

    // Main Window
    SplitView {
        anchors.fill: parent

        // TODO: QFileSystemWatcher for local container directory
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
                updateView();
            }
            onActivated: {
                _containerModel.setCurrentContainer(containerList.currentRow)
                updateView();
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



    function updateView()
    {
        viewLoader.source = "";

        if (containerList.currentRow < 0)
            viewLoader.source = "Welcome.qml";

        var container = _containerModel.get(containerList.currentRow)

        if (container.encrypted)
            viewLoader.source = "ContainerEncrypted.qml";
        else
            viewLoader.source = "ContainerDecrypted.qml";
    }
}
