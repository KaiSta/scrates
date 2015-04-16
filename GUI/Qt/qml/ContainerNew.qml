import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2

import tempest.RandomSeedGenerator 1.0

Item {
    RandomSeedGenerator {
        id: randomSeedGenerator
    }

    /*
    Timer {
        id: timer
        interval: 1000
        running: true
        repeat: true
        onTriggered: randomSeedGenerator.time()
   }
   */

    SystemPalette { id: palette }
    MouseArea {
        anchors.fill: parent
        onPositionChanged: randomSeedGenerator.randomSeed(mouseX, mouseY)
        hoverEnabled: true
    }

    GridLayout {
        columns: 2
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 10
        }
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
            readOnly: true
            text: randomSeedGenerator.seed
        }

        Label {
            text: qsTr("Cloud Service:")
        }

        ComboBox{
            currentIndex: 0
            model: providersModel
            textRole: "placeholderName"
            // onCurrentIndexChanged: console.debug(providersModel.get(currentIndex).placeholder + ", " + providersModel.get(currentIndex).location)
            onCurrentIndexChanged: pathText.text = providersModel.get(currentIndex).location
        }

        Label {
            text: qsTr("Directory:")
        }
        RowLayout {
            TextField {
                id: pathText
                Layout.fillWidth: true
                text: providersModel.get(0).location
            }

            Button {
                text: qsTr("Open...")
                onClicked: containerPathFileDialog.open()
                // TODO: open container path if exists
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

        RowLayout {
            id: buttonRow
            spacing: 5
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                right: parent.right
                leftMargin: 10
                rightMargin: 5
            }

            CheckBox {
                id: isDecrypted
                text: qsTr("Mount container after saving")
                checked: true
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "Save"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    if (_containerModel.add(nameText.text, pathText.text, passwordText.text, !isDecrypted.checked))
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
        folder: pathText.text // BUG
        selectFolder: true
        onAccepted: pathText.text = fileUrl
    }
}
