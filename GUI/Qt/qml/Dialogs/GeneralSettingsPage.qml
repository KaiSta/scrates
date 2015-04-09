import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.0

Item {
    clip: true

    ColumnLayout {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 10
        }

        Label {
            text: qsTr("Location for container key files:")
            Layout.alignment: Qt.AlignTop
        }
        RowLayout {
            Layout.fillWidth: true
            TextField {
                placeholderText: qsTr("Location")
                text: _applicationDirPath
                Layout.fillWidth: true
            }
            Button {
                text: qsTr("Choose...")
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
                           placeholderText: qsTr("Location")
                           text: _homeDirPath
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
                RowLayout {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                   TextField {
                       placeholderText: qsTr("Location")
                       text: _homeDirPath
                       Layout.fillWidth: true
                   }
                   Button {
                       text: qsTr("Choose...")
                   }
                }
            }
        }
    }
}
