import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.0
import "../UIComponents"

Window {
    id: settingsDialog
    title: qsTr("Preferences")
    width: 480
    minimumWidth: 480
    height: 420
    minimumHeight: 400
    modality: Qt.ApplicationModal

    ColumnLayout {
        anchors.fill: parent
        TabView {
            id: tabView
            anchors {
                fill: parent
                margins: 5
            }
            Tab {
                id: generalPageTab
                title: qsTr("General")
                GeneralSettingsPage { }
            }
            Tab {
                id: providersPageTab
                title: qsTr("Providers")
                ProvidersSettingsPage { }
            }
        }
        BottomBar {
            height: buttonBar.height + 10
            ButtonBar {
                id: buttonBar
                Item { Layout.fillWidth: true }
                Button {
                    text: "OK"
                    onClicked: saveSettings()
                }
                Button {
                    text: "Cancel"
                    onClicked: settingsDialog.close()
                }
            }
        }
    }

    function saveSettings() {
        if (generalPageTab.item) generalPageTab.item.save();
        if (providersPageTab.item) providersPageTab.item.save();
        settingsDialog.close();
    }

    // unused
    function updateWindowHeight() {
        var currentTabIdx = tabView.currentIndex;
        if (currentTabIdx == 0) settingsWindow.height = generalPageTab.item.columnHeight + 100;
        if (currentTabIdx == 1) settingsWindow.height = providersPageTab.item.columnHeight + 100;
    }
}
