import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2

Item {
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
            id: passwordText
            // echoMode: TextInput.Password
            Layout.fillWidth: true

            onTextChanged: _pwStrengthChecker.calcStrength(passwordText.text)
            validator: RegExpValidator {
                // regExp: /[abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!$%&/()=?+-_,.#<>]*/
                regExp: /[a-zA-Z0-9!$%&/()=?+-_,.#<>]*/
            }

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
            id: randomSeedText
            Layout.fillWidth: true
        }

        Label {
            text: qsTr("Container Path:")
        }

        RowLayout {
            TextField {
                id: containerPathText
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("...")
                onClicked: messageDialog.show(qsTr("Button 1 pressed"))
            }

        }



        RowLayout {
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignRight

            Button {
                text: qsTr("Cancel")
                onClicked: messageDialog.show(qsTr("Button 1 pressed"))
            }

            Button {
                text: qsTr("Save")
                onClicked: messageDialog.show(qsTr("Button 1 pressed"))
            }
        }




        Button {
            text: qsTr("Add Container")
            onClicked: _containerModel.addContainer("cococontainer", "hello")
        }


    }

}


