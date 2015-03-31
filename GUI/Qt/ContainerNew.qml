import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0
import tempest.Container 1.0

Item {
    Container {
        id: container
        name: "testHELLO"
        password: "testPASSWORD"
        path: "testPATH"
    }

    SystemPalette { id: palette }
    Settings {
        property alias text: pathText.text
    }

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
            id: nameText
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
            // echoMode: TextInput.PasswordEchoOnEdit
            echoMode: (this.focus ? TextInput.Normal : TextInput.Password)
            Layout.fillWidth: true
            onTextChanged: _pwStrengthCheckerModel.calcStrength(this.text)
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
            text: qsTr("Directory:")
        }
        RowLayout {
            TextField {
                id: pathText
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Open...")
                onClicked: containerPathFileDialog.open()
            }
        }
    }

    Rectangle {
        id: bottomBar
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: buttonRow.height * 1.2
        color: Qt.darker(palette.window, 1.1)

        Row {
            id: buttonRow
            spacing: 6
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 12
            width: parent.width

            Button {
                text: "Save"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    // _containerModel.add(container)
                    if (_containerModel.add(nameText.text, pathText.text, passwordText.text))
                        viewLoader.source = "Welcome.qml"
                    else
                        messageDialog.show("TODO: Fehler")
                }
                enabled: (isValid()) ? true : false
            }
            Button {
                text: "Cancel"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: viewLoader.source = "Welcome.qml"
            }
        }
    }

    // Returns true, if the form is valid
    function isValid()
    {
        return (nameText.text.length && passwordText.text.length)
    }

    FileDialog {
        id: containerPathFileDialog
        title: "Choose container directory"
        modality: Qt.NonModal
        folder: "file:///Users" // TODO: Cloud Service Path (e.g. Dropbox)
        selectFolder: true
        onAccepted: pathText.text = fileUrl
    }




}
