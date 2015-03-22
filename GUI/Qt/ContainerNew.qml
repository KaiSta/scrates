import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2

Item {
    //width: 640
    //height: 480
    //width: parent
    //height: parent

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
            // echoMode: TextInput.Password
            Layout.fillWidth: true
            onTextChanged: _pwStrengthChecker.calcStrength(passwordInput.text)
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
                    implicitHeight: 24
                }
                progress: Rectangle {
                    color: _pwStrengthChecker.color

                    Text {
                        anchors.centerIn: parent
                        text: _pwStrengthChecker.message
                    }
                }
            }
        }

        Label {
            text: qsTr("Random Seed:")
        }

        TextField {
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Press Me 1")
            onClicked: messageDialog.show(qsTr("Button 1 pressed"))
        }

        Button {
            text: qsTr("Add Container to List")
            onClicked: _containerModel.addContainer("muha", "hello")
        }
        Button {
            text: qsTr("Delete")
            // onClicked: messageDialog2.open()
            onClicked: _containerModel.deleteContainer(containerList.currentRow)
        }
    }
}
