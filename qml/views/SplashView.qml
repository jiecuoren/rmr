import QtQuick 1.0
import MReader 1.0

MrView {
    id: splashview
    viewId: appState.kSplashView
    width: screenWidth
    height: screenHeight

    property bool needUserInfo: false

    Rectangle{id:bk; anchors.fill: parent; color: "black"}

    Image {
        id: logo
        y: parent.height/3
        source: "../resources/icon/splash_screen.png"
        anchors.horizontalCenter: parent.horizontalCenter

    }

    Text {
        id: loadtext
        text: "loading..."
        color: "white"
        anchors{
            top: logo.bottom
            margins: 8
            horizontalCenter: parent.horizontalCenter
        }
        font.pointSize: 6
    }

    Timer {
        id: timer
        interval: 1000 //1 seconds
        repeat:  false
        running: false

        onTriggered: {
            if(needUserInfo) {
                viewManager.activeView(appState.kLoginView, viewManager.noAnimation)
            }
            else {                
                MRApplication.controller.handleCommand(opIdToMainView, viewManager.fade)
            }

        }
    }

    Component.onCompleted: {
//        application.getAccMgr().clearDatabase();

        var userInfo  =
        {
            userName: "",
            password: ""
        };

        if(application.getAccMgr().loadUserInfo(userInfo))
        {
            application.userName = userInfo.userName;
            application.password = userInfo.password;
            MRApplication.controller.opmlConfig().initUserFolder(userInfo.userName);
            MRApplication.controller.opmlConfig().loadOpml();
            MRApplication.controller.handleCommand(opIdInitialize);
            console.log("userName is " + application.userName + "password is " + application.password);
        }
        else
        {
            application.userName = "";
            application.password = "";
            needUserInfo = true;
        }

        timer.running = true;

    }
}
