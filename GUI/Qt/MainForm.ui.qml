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

    RowLayout {
        anchors.centerIn: parent

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
        TextField {
            id: passwordInput
            echoMode: TextInput.Password
            placeholderText: "Password"
        }
        ProgressBar {
            id: pwProgressBar
            minimumValue: 0
            maximumValue: 1
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
    }
}
