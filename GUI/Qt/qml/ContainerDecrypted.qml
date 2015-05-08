import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

Item {
    SystemPalette { id: palette }
    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 0

            TextArea {
                id: history
                Layout.fillWidth: true
                Layout.fillHeight: true
                frameVisible: false
                readOnly: true
                text: _containerModel.currentContainer().history
                wrapMode: Text.WrapAnywhere
                textFormat: TextEdit.RichText
                textMargin: 5
            }

            Rectangle {
                id: bottomBar
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: buttonRow.height * 1.2
                color: Qt.darker(palette.window, 1.1)

                Row {
                    id: buttonRow
                    spacing: 5
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: 10
                    }
                    width: parent.width

                    Button {
                        text: "Clear"
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: _containerModel.currentContainer().history = ""
                        enabled: (_containerModel.currentContainer().history.length ? true : false)
                    }
                    Button {
                        text: "Export"
                        anchors.verticalCenter: parent.verticalCenter
                        enabled: (_containerModel.currentContainer().history.length ? true : false)
                        onClicked: _containerModel.currentContainer().exportHistory("/Users/jochen/Desktop/test")
                    }
                }
            }
        }

        ColumnLayout {
            Button {
                text: qsTr("Unmount")
                onClicked: unmount()
            }

            Button {
                text: qsTr("Open")
                tooltip: qsTr("Open container directory")
                onClicked: _containerModel.currentContainer().openDirectory()
                enabled: (containerList.currentRow > -1 & containerList.currentRow < containerList.rowCount ? true : false)
            }

            Button {
                text: qsTr("Sync")
                onClicked: sync()
            }
        }
    }

    function unmount() {
        _containerModel.currentContainer().unmount();
        updateView();
    }
    function sync() {
        _containerModel.currentContainer().sync();
    }

    function test() {
        //QStringList
        var s = _containerModel.currentContainer().strings;
        s = ["bbbb"];
        s[s.length] = "Lemon";

        console.log(s[0]);
        console.log(s.length);
    }
}

