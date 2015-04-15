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
    height: 400
    minimumHeight: 400
    modality: Qt.ApplicationModal

    ColumnLayout {
        anchors.fill: parent
        TabView {
            anchors {
                fill: parent
                margins: 5
            }
            Tab {
                id: generalPage
                title: qsTr("General")
                GeneralSettingsPage {}
            }
            Tab {
                id: providersPage
                title: qsTr("Providers")
                ProvidersSettingsPage {}
            }
        }
        BottomBar {
            height: 40
            ButtonBar {
                Button {
                    text: "Save"
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
        // TODO
        _settings.setValue("Settings/")
        if (true)
            settingsDialog.close()
    }


}
