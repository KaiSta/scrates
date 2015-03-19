import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2

Item {
    //width: 640
    //height: 480
    width: parent
    height: parent

    property alias button2: button2
    property alias button1: button1
    property alias passwordInput: passwordInput

    GridLayout {
        columns: 2
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10
        rowSpacing: 10
        columnSpacing: 10


        Label {
            text: qsTr("Name:")
        }

        TextField {
            Layout.fillWidth: true
        }

        Label {
            text: qsTr("Password:")
        }

        TextField {
            id: passwordInput
            echoMode: TextInput.Password
            Layout.fillWidth: true
        }

        Label {
            text: qsTr("Strength:")
        }

        ProgressBar {
            id: pwProgressBar
            minimumValue: 0
            maximumValue: 1
            Layout.fillWidth: true
            value: _pwStrengthChecker.strength

            style: ProgressBarStyle {
                background: Rectangle {
                    color: "lightgray"
                    implicitWidth: 200
                    implicitHeight: 24
                }
                progress: Rectangle {
                    color: _pwStrengthChecker.color

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        text: _pwStrengthChecker.message
                    }
                }
            }
        }

        Button {
            id: button1
            text: qsTr("Press Me 1")
        }

        Button {
            id: button2
            text: qsTr("Press Me 2")
        }

    }

    RowLayout {
        anchors.centerIn: parent


    }

}
