import QtQuick 1.0
import MReader 1.0

Rectangle {
    id: container
    width: 360
    height: 640
    anchors.fill: parent
    color: "transparent"
    scale: 0.0
    z: 1000

    property string sohuNewBlog: "http://api.t.sohu.com/statuses/update.json" //status
    property string shareUserName
    property string sharePassword
    property string headerValue: "Basic " + Qt.btoa( shareUserName + ":" + sharePassword );

    property alias text: inputArea.text
    property variant dialog
    property string dialogType: ""

    signal shareToSinaClicked
    signal shareToSohuClicked
    signal shareToTwitterClicked

    signal shareFinished
    signal shareError
    signal cancelClicked

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
         width: 360
         height: 300
         opacity: 0.9
         anchors.horizontalCenter: parent.horizontalCenter
         anchors.verticalCenter: parent.verticalCenter

         Text {
             id: lable
             width: 300
             height: 30
             color: "white"
             font{pixelSize: 20}
             anchors {top: backGround.top; topMargin: 10; left:  backGround.left; leftMargin: 10}
             text: "Input comment"
         }

         BorderTextInput {
             id: inputArea
             width: 340
             height: 180
             textInputSize: 25
             anchors.left: backGround.left
             anchors.leftMargin: 10
             anchors.top:  lable.bottom
         }

         Rectangle {
             id: sharePanel
             width: parent.width
             height: 60
             color: "transparent"
             anchors { top: inputArea.bottom; topMargin: 5 }

             Button {
                 id: shareToSina

                 imgWidth: 50
                 image: "../resources/icon/sina-logo.png"
                 image_press: "../resources/icon/sina-logo.png"
                 anchors{
                     left:  parent.left
                     leftMargin: 25
                     verticalCenter: parent.verticalCenter
                 }

                 onClicked: {
                     console.log("shareToSina clicked");
                     shareToSinaClicked();
                 }
             }

             Button  {
                 id: shareToSohu

                 imgWidth: 50
                 image: "../resources/icon/sohu-logo.png"
                 image_press: "../resources/icon/sohu-logo.png"
                 anchors{
                     horizontalCenter: parent.horizontalCenter
                     verticalCenter: parent.verticalCenter
                     topMargin: 5
                     top: parent.top
                 }

                 onClicked: {
                     console.log("shareToSohu clicked");
                     shareToSohuClicked();
                 }
             }

             Button  {
                 id: shareToTwitter
                 scale: 0.95
                 imgWidth: 50
                 image: "../resources/icon/twitter-logo.png"
                 image_press: "../resources/icon/twitter-logo.png"
                 anchors{
                     right:  parent.right
                     rightMargin: 35
                     verticalCenter: parent.verticalCenter
                 }

                 onClicked: {
                     console.log("shareToTwitterClicked clicked");
                     shareToTwitterClicked();
                 }
             }
         }

         Rectangle {
             id: cancleBtnBk
             width: backGround.width
             height: 50
             anchors.top: backGround.bottom
             anchors.horizontalCenter: backGround.horizontalCenter

             color: "black"

             LoginButton {
                 id: cancel
                 anchors.fill: parent
                 text: "Cancel"
                 onClicked: {
                     cancelClicked();
                 }
             }
         }


    }

    WaitIndicator {
        id: waitIndicator
        z: 100
        anchors.centerIn: parent
        show: false
    }

    SohuMiniBlog {
        id: sohu

        onLoadingFinished: {
            console.log("Finished!");
            waitIndicator.show = false
            shareFinished();
        }

        onLoadingError: {
            console.log("Error in SohuMiniBlog");
            waitIndicator.show = false
            shareError();
        }
    }


    onShareToSohuClicked: {
        console.log("onShareToSohuClicked");

        var userInfo  =
        {
            userName: "",
            password: ""
        };

        if(application.getAccMgr().loadSohuUserInfo(userInfo))
        {
            shareUserName = userInfo.userName;
            sharePassword = userInfo.password;
            postNewblog(inputArea.text);

        }
        else
        {
            createDialog();
            shareUserName = "";
            sharePassword = "";
            dialogType = "sohu"
        }


    }

    onShareToSinaClicked: {
        console.log("onShareToSinaClicked");
    }

    onShareToTwitterClicked: {
        console.log("onShareToTwitterClicked");
        dialogType = "twitter"
        application.getAccMgr().clearSohuDatabase();
    }

    function createDialog()
    {
        console.log("page view createDialog");

        var component;
        component = Qt.createComponent("../component/Dialog.qml");

         if (component.status == Component.Ready) {
             dialog = component.createObject(container);
             dialog.label1Text = "Input user name:"
             dialog.label2Text = "Input password:"
             dialog.leftBtnText = "OK"
             dialog.feedAddress = "mreader123@163.com"
             dialog.feedName = "mreader1234"
             dialog.yesClicked.connect(dialogYesClicked);
             dialog.noClicked.connect(dialogNoClicked);
         }

         backGround.visible = false
    }

    function dialogYesClicked(type)
    {
        console.log("page view dialogYesClicked " + dialog.feedAddress + "   " + dialog.feedName);

        shareUserName = dialog.feedAddress;
        sharePassword = dialog.feedName;

        var userInfo = {
            userName: shareUserName,
            password: sharePassword
        }

        application.getAccMgr().createSohuUserInfo(userInfo)

        dialog.yesClicked.disconnect(dialogYesClicked);
        dialog.noClicked.disconnect(dialogNoClicked);
        dialog.destroy();

        backGround.visible = true

        if("sohu" == dialogType)
        {
            postNewblog(inputArea.text);
        }

    }

    function dialogNoClicked()
    {
        console.log("page view dialogNoClicked");
        dialog.yesClicked.disconnect(dialogYesClicked);
        dialog.noClicked.disconnect(dialogNoClicked);
        dialog.destroy();

        backGround.visible = true
    }

    function postNewblog(content) {
        console.log("postNewblog");
        waitIndicator.show = true
        sohu.setHeader("Authorization", headerValue);
        sohu.postPairs(sohuNewBlog, "status", encodeURIComponent(content));
    }

    SequentialAnimation {
        id: enlargerAnimation
        running: false

        PropertyAnimation {target: container; property: "scale";
            from: 0.0; to: 1.0; duration: 400; easing.type: Easing.InQuad }
    }

    Component.onCompleted: {
        enlargerAnimation.running = true;
    }
}
