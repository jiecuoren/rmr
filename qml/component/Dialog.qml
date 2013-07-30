import Qt 4.7

Rectangle {
    id: container

    anchors.fill: parent
    color: "transparent"
    scale: 0.5
    property alias feedAddress: inputArea.text
    property alias feedName: inputArea2.text

    property alias label1Text: lable.text
    property alias label2Text: lable2.text

    property alias leftBtnText: yesBtn.text

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
         height: 200
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
        text: "Input feed address: "
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

    Text {
        id: lable2
        width: 300
        height: 30
        color: "white"
        font{pixelSize: 20}
        anchors {top: inputArea.bottom; topMargin: 10; left:  backGround.left; leftMargin: 10}
        text: "Input feed name: "
    }

    BorderTextInput {
        id: inputArea2
        width: 300
        height: 30
        anchors.left: backGround.left
        anchors.leftMargin: 10
        anchors.top: lable2.bottom
        anchors.topMargin: 10
    }

    LoginButton {
        id: yesBtn
        width: backGround.width / 2
        height: 30
        anchors.top: backGround.bottom
        anchors.left: backGround.left
        text: "Add"
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
        text: "Cancel"
        onClicked: {
            container.noClicked();
        }
    }

    SequentialAnimation {
        id: enlargerAnimation
        running: false

        PropertyAnimation {target: container; property: "scale";
            from: 0.5; to: 1.0; duration: 400; easing.type: Easing.InQuad }
    }


    Component.onCompleted: {
        enlargerAnimation.running = true;
    }
}
