import QtQuick 1.0

Item {
    id: container

    width: checkIcon.width + checkText.width + 30
    height: checkIcon.height + 10

    property bool checked: false
    property alias text: checkText.text

    Image {
        id: checkIcon
        source: checked ? "../resources/icon/check_no.png" : "../resources/icon/check_yes.png"
        width: 20
        height: 20

        anchors {left: parent.left; top:  parent.top}

        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("check box clicked!" + checked)
                checked = !checked;
            }
        }
    }

    Text {
        id: checkText
        font.pixelSize: 20
        color: "white"
        anchors {left: checkIcon.right; leftMargin: 15; top:  parent.top}
    }

}
