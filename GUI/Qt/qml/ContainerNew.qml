import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2

Item {
    Component.onCompleted: nameText.forceActiveFocus()
    property string syncLocationProvider: ""

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
        onPositionChanged: _randomSeedGeneratorModel.randomSeed(mouseX, mouseY)
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
            maximumLength: 32
            validator: RegExpValidator {
                regExp: /[a-zA-Z0-9_-]*/ // TODO
            }
            onTextChanged: if (isCreateNewDirectory.checked) updateSyncLocation()
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
            text: _randomSeedGeneratorModel.seed
        }

        Label {
            text: qsTr("Sync Location:")
        }
        RowLayout {
            ComboBox{
                currentIndex: 0
                model: providersModel
                textRole: "placeholderName"
                onCurrentIndexChanged: {
                    syncLocationProvider = providersModel.get(currentIndex).location
                    syncLocationText.text = providersModel.get(currentIndex).location
                    syncLocationText.forceActiveFocus()
                    syncLocationText.selectAll()
                    if (isCreateNewDirectory.checked) updateSyncLocation()
                }
            }
            TextField {
                id: syncLocationText
                Layout.fillWidth: true
                text: providersModel.get(0).location
                readOnly: true
            }
        }

        Label { }
        CheckBox {
            id: isCreateNewDirectory
            text: qsTr("Creates new directory for syncing container files")
            checked: true
            onCheckedChanged: (this.checked ? updateSyncLocation() : syncLocationText.text = syncLocationProvider)
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
                id: isMounted
                text: qsTr("Mount container after saving")
                checked: true
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "Add"
                anchors.verticalCenter: parent.verticalCenter
                onClicked: createContainer()
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
    function isValid() {
        return (nameText.text.length && passwordText.text.length)
    }

    function updateSyncLocation() {
        syncLocationText.text = syncLocationProvider + nameText.text;
        if (nameText.length) syncLocationText.text += "/";
    }

    function createContainer() {
        if (isValid()) {
            if (_containerModel.create(nameText.text, passwordText.text, syncLocationText.text, isMounted.checked)) {
                _randomSeedGeneratorModel.setSeed();
                viewLoader.source = "Welcome.qml";
            }
            else {
                console.log("error: createContainer");
            }
        }
    }
}
