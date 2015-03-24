import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

Item {


RowLayout {
    TextField {
        text:_containerModel.get(containerList.currentRow).name
        onTextChanged: _containerModel.get(containerList.currentRow).name = text
    }
}


}
