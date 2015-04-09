import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.0

Window {
    id: settingsDialog
    title: qsTr("Preferences")
    width: 480
    minimumWidth: 480
    height: 400
    minimumHeight: 400
    modality: Qt.ApplicationModal

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
}
