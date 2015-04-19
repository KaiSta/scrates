import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

Item {
    clip: true
    property alias columnHeight: columnLayout.height

    FileDialog {
        id: containerLocationFileDialog
        title: qsTr("Choose directory")
        selectFolder: true
        folder: containerLocationText.text
        onAccepted: {
            containerLocationText.text = fileUrl
            containerLocationText.forceActiveFocus()
            containerLocationText.selectAll()
        }
    }

    FileDialog {
        id: vhdLocationFileDialog
        title: qsTr("Choose directory")
        selectFolder: true
        onAccepted: {
            loader.item.vhdLocation.text = fileUrl
            loader.item.vhdLocation.forceActiveFocus()
            loader.item.vhdLocation.selectAll()
        }
    }

    ColumnLayout {
        id: columnLayout
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 10
        }

        Label {
            text: qsTr("Location for container key files (.cco):")
            Layout.alignment: Qt.AlignTop
        }
        Text {
            text: qsTr("(Info) Usually you don't have to change this directory. If so, you've to move your container files manually.")
            // TODO: If containerList not empty -> restart required after location changed
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            font.italic: true
            font.pixelSize: 10
        }
        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: containerLocationText
                placeholderText: qsTr("Container location")
                text: _settings.value("Container/containerLocation")
                Layout.fillWidth: true
            }
            Button {
                text: qsTr("Choose...")
                onClicked: containerLocationFileDialog.open()
            }
        }

        Label {
            id: titleLabel
            text: qsTr("Directory for mounted containers:")
        }

        Loader {
            id: loader
            anchors.top: titleLabel.bottom
            anchors.topMargin: 5
            Layout.fillWidth: true
            sourceComponent: (Qt.platform.os === "windows"  ? winComponent : otherComponent)
        }

        // Windows
        Component {
            id: winComponent
            Item {
                property alias vhdLocation : vhdLocationText
                ColumnLayout {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    ExclusiveGroup { id: mountTypeGroup }
                    RadioButton {
                       text: qsTr("Virtual Harddisk (Windows only)")
                       exclusiveGroup: mountTypeGroup
                    }
                    RadioButton {
                       id: localRadioButton
                       text: qsTr("Local directory")
                       checked: true
                       exclusiveGroup: mountTypeGroup
                    }

                    RowLayout {
                       visible: localRadioButton.checked
                       Layout.fillWidth: true
                       TextField {
                           id: vhdLocationText
                           placeholderText: qsTr("Mount location")
                           text: _settings.value("Container/vhdLocation")
                           Layout.fillWidth: true
                       }
                       Button {
                           text: qsTr("Choose...")
                       }
                    }
                }
            }
        }

        // Others
        Component {
            id: otherComponent
            Item {
                property alias vhdLocation : vhdLocationText
                RowLayout {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                   TextField {
                       id: vhdLocationText
                       placeholderText: qsTr("Mount location")
                       text: _settings.value("Container/vhdLocation")
                       Layout.fillWidth: true
                   }
                   Button {
                       text: qsTr("Choose...")
                       onClicked: vhdLocationFileDialog.open()
                   }
                }
            }
        }
    }

    function save() {
        // TODO: validate
        _settings.setValue("Container/containerLocation", containerLocationText.text);
        _settings.setValue("Container/vhdLocation", loader.item.vhdLocation.text);
    }
}
