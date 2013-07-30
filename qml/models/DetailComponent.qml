import QtQuick 1.0

Item {
    id: container

    property string textStr: "name.text"
    property alias component: detailComponent

    Component {
        id: detailComponent

        Item {
            id: rootItem

            Text {
                id: name
                anchors.centerIn: parent
                text: textStr
                font.pixelSize: 50
            }
        }

    }

}

