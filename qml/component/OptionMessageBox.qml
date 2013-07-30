import Qt 4.7

Rectangle {
    id: container

    anchors.fill: parent

    color: "transparent"
    scale: 0.5
    signal yesClicked
    signal noClicked

    property alias text: text1.text

    MouseArea{
        id: mask
        anchors.fill: parent
        onClicked: {
        }
    }

    Rectangle {
         id: backGround
         color: "lightblue"
         border.color: "gray"
         border.width: 3
         radius: 10
         width: 320
         height: 130
         opacity: 0.9
         anchors.horizontalCenter: parent.horizontalCenter
         anchors.verticalCenter: parent.verticalCenter
    }

    Image {
        id: image
        source: "../resources/icon/messagebox_info.png"
        width: backGround.width/2 - 60
        height: backGround.height - 10
        anchors.left: backGround.left
        anchors.leftMargin: 10
        anchors.verticalCenter: backGround.verticalCenter
    }

    Text {
        id: text1
        width:backGround.width/2 + 60
        height:backGround.height
        color: "black"
        font.bold :true
        font.pixelSize: 28
        wrapMode: Text.WordWrap
        anchors.right: backGround.right
        anchors.top: backGround.top
        anchors.leftMargin: 40
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    LoginButton {
        id: yesBtn
        width: backGround.width / 2
        height: 40
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
        height: 40
        anchors.top: backGround.bottom
        anchors.right: backGround.right
        text: "No"
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
