import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2

// UNUSED

/*Window*/
/*MessageDialog*/
Dialog {
    id: newContainerDialog
    title: qsTr("Create new container")
    width: 480
    height: 320
    standardButtons: StandardButton.Save | StandardButton.Cancel
    onVisibleChanged: if (newContainerDialog.visible) name.forceActiveFocus()


    GridLayout {
        id: grid
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
            id: name
            Layout.fillWidth: true

            validator: RegExpValidator {
                regExp: /[a-zA-Z0-9_-]*/ // TODO
            }
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
            value: _pwStrengthCheckerModel.strength

            style: ProgressBarStyle {
                background: Rectangle {
                    color: "lightgray"
                    implicitHeight: 24
                }
                progress: Rectangle {
                    color: _pwStrengthCheckerModel.color

                    Text {
                        anchors.centerIn: parent
                        text: _pwStrengthCheckerModel.message
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
                text: qsTr("Open...")
                onClicked: fileDialog.open()
            }
        }
    }
}




