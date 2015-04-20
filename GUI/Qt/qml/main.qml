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
        // close.accepted = false
        // closeApplicationMessageDialog.open()
    }

    XmlListModel {
        id: providersModel
        source: "file://" + _applicationDirPath + "/providers.xml"
        query: "/providers/provider"
        XmlRole {name: "placeholder"; query: "@placeholder/string()"}
        XmlRole {name: "placeholderName"; query: "substring-after(@placeholder/string(), '$')"}
        XmlRole {name: "location"; query: "@location/string()"}
    }

    // Message Dialog: Deleting Container
    MessageDialog {
        id: closeApplicationMessageDialog
        modality: Qt.WindowModal
        title: qsTr("Quit application")
        text: qsTr("Quit application")
        informativeText: qsTr("Are you sure to quit the application? All opened containers will be synced and unmounted.")
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            _settings.setValue("ApplicationWindow/width", width)
            _settings.setValue("ApplicationWindow/height", height)
            Qt.quit()
        }
    }

    // Message Dialog: Deleting Container
    MessageDialog {
        id: removeContainerMsgDialog
        modality: Qt.WindowModal
        title: qsTr("Removing selected container")
        text: qsTr("Removing container")
        informativeText: qsTr("Are you sure to delete the selected container?")
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            _containerModel.remove(containerList.currentRow)
            viewLoader.source = "Welcome.qml"
        }
    }

    FileDialog {
        id: openContainerFileDialog
        title: qsTr("Choose container")
        modality: Qt.NonModal
        nameFilters: [ "Containerfiles (*.cco)" ]
        onAccepted: _containerModel.open(fileUrl)
    }

    FileDialog {
        id: importContainerFileDialog
        title: qsTr("Choose container")
        modality: Qt.NonModal
        nameFilters: [ "Containerfiles (*.cco)" ]
        //onAccepted: console.log(Qt.resolvedUrl(fileUrl).toString())
    }

    SettingsDialog {
        id: settingsDialog
    }

    Action {
        id: openAction
        onTriggered: openContainerFileDialog.open()
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
                enabled: false
                text: qsTr("&Import")
                shortcut: "Ctrl+I"
                onTriggered: importContainerFileDialog.open()
            }
            MenuItem {
                enabled: false
                text: qsTr("&Export")
                shortcut: "Ctrl+E"
            }
            MenuSeparator { }
            MenuItem {
                text: qsTr("E&xit")
                onTriggered: closeApplicationMessageDialog.open()
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
            Item { Layout.fillWidth: true }
        }
    }

    // In progress. TODO.
    MessageBar {
        id: messageBar
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

    function updateView() {
        viewLoader.source = ""

        if (containerList.currentRow < 0)
            viewLoader.source = "Welcome.qml";

        var container = _containerModel.get(containerList.currentRow)

        if (container.open)
            viewLoader.source = "ContainerDecrypted.qml";
        else
            viewLoader.source = "ContainerEncrypted.qml";
    }

    function importContainer(fileUrl) {
        //console.log(fileUrl);
        //console.log("resolved:" + Qt.resolvedUrl(fileUrl));
    }
}
