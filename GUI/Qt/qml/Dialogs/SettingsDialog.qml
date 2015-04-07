import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.0

/*Window*/
/*MessageDialog*/
Window {
    title: "Preferences"
    width: 480
    height: 320
    modality: Qt.ApplicationModal

    TabView {
        anchors.fill: parent
        anchors.margins: 8
        Tab {
            id: controlPage
            title: "General"
            // TODO
        }
    }
}

