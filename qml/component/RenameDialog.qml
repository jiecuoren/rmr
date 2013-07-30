import Qt 4.7

Rectangle {
    id: container

    anchors.fill: parent
    color: "transparent"

    property alias renameText: inputArea.text

    signal yesClicked
    signal noClicked

    MouseArea{
        id: mask
        anchors.fill: parent
        onClicked: {
        }
    }

    Rectangle {
         id: backGround
         color: "black"
         border.color: "gray"
         border.width: 3
         radius: 10
         width: 320
         height: 120
         opacity: 0.9
         anchors.horizontalCenter: parent.horizontalCenter
         anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        id: lable
        width: 300
        height: 30
        color: "white"
        font{pixelSize: 20}
        anchors {top: backGround.top; topMargin: 10; left:  backGround.left; leftMargin: 10}
        text: "Rename to: "
    }

    BorderTextInput {
        id: inputArea
        width: 300
        height: 30
        anchors.left: backGround.left
        anchors.leftMargin: 10
        anchors.top:  lable.bottom
        anchors.topMargin: 10
    }

    LoginButton {
        id: yesBtn
        width: backGround.width / 2
        height: 30
        anchors.top: backGround.bottom
        anchors.left: backGround.left
        text: "Yes"
        onClicked: {
            container.yesClicked();
        }
    }

    LoginButton {
        id: noBtn
        width: backGround.width / 2
        height: 30
        anchors.top: backGround.bottom
        anchors.right: backGround.right
        text: "No"
        onClicked: {
            container.noClicked();
        }
    }
}
