import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1




TableView {

//property TableView containerTable: containerTable




    id: containerTable
    model: _containerModel
    frameVisible: false
    Layout.minimumWidth: 100
    Layout.maximumWidth: 300

    TableViewColumn {
        title: qsTr("Container")
        role: "name"
    }




}

/*
ListView {
    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
    focus: true
    model: containerModel
    delegate: Text { text: "Animal: " + type + ", " + size }
}
*/
