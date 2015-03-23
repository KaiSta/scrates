import QtQuick 2.0

Item {


Text {
    text: _containerModel.get(containerList.currentRow, 2)
    font.family: "Helvetica"
    font.pointSize: 24
    color: "red"
}


}
