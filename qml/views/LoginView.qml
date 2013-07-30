import QtQuick 1.0
import MReader 1.0
import "../component"

MrView {
    id: aboutView
    width: screenWidth
    height: screenHeight

    viewId: appState.kLoginView

    property variant userInfo:{ "userName": "annie.mreader@gmail.com", "password": "Test2011" };

    MouseArea {
        id: mask
        anchors.fill: parent

        onClicked: {
            console.log("clicked on login view");
        }
    }

    Rectangle{id:bk; anchors.fill: parent; color: "black"}

    Image {
        id: googleLogo
        width: 250
        height: (250/sourceSize.width) * sourceSize.height
        source: "../resources/icon/google_small.PNG"
        anchors {top: parent.top; topMargin: 80; horizontalCenter: parent.horizontalCenter}
    }

    Text {
        id: userNameLable
        width: 300
        height: 30
        color: "white"
        font{pixelSize: appState.titleFontSize}
        anchors {top: googleLogo.bottom; topMargin: 30; horizontalCenter: parent.horizontalCenter}
        text: "Input your email: "
    }

    BorderTextInput {
        id: userName
        width: 300
        height: 30
        border.width: 5
        border.color: "transparent"
        radius: 2
        text: userInfo.userName
        anchors {top: userNameLable.bottom; topMargin: 5; left: userNameLable.left}
    }

    Text {
        id: passwordLable
        width: 300
        height: 30
        color: "white"
        font{pixelSize: appState.titleFontSize}
        anchors {top: userName.bottom; topMargin: 10; horizontalCenter: parent.horizontalCenter}
        text: "Input password: "
    }

    BorderTextInput {
        id: password
        width: 300
        height: 30
        echoMode: TextInput.Password
        border.width: 5
        border.color:  focus ?  "yellow" : "transparent"
        radius: 2
        text: userInfo.password
        anchors {top: passwordLable.bottom; topMargin: 5; left: userNameLable.left}
    }

    CheckBox {
        id: checkBox
        text: "Keep Login"
        anchors {top: password.bottom; topMargin: 25; horizontalCenter: parent.horizontalCenter}
    }

    LoginButton {
        id: loginBtn
        width: 130
        height: 40
        text: "Login"
        anchors{top: checkBox.bottom; topMargin: 20; left: userNameLable.left}
        onClicked: {
            console.log("log in btn clicked");
            waitIndicator.show = true;
            focus = true;
            google.doLogin(userName.text, password.text, checkBox.checked);
        }
    }

    LoginButton {
        id: goOnBtn
        width: 130
        height: 40
        text: "Continue"
        anchors{top: checkBox.bottom; topMargin: 20; left: loginBtn.right; leftMargin: 40}
        onClicked: {
            console.log("Continue btn clicked");
            MRApplication.controller.handleCommand(opIdInitialize);
            MRApplication.controller.handleCommand(opIdToMainView, viewManager.fade)
        }
    }

    Text {
        id: registerText
        color: "white"
        font.pixelSize: 20
        textFormat: Text.RichText
        text: "No google account? <a href=\"https://www.google.com/accounts/NewAccount\">Click here!</a>"
        onLinkActivated: {
            console.log(link + " link activated")
            Qt.openUrlExternally(link)
        }
        anchors{bottom: parent.bottom; bottomMargin: 40; right: parent.right; rightMargin: 5}
   }

    Button{
        id: exit
        imgWidth: appState.toolBarHeight
        image: "../resources/icon/exit.png"
        image_press: "../resources/icon/exit-press.png"

        anchors{
            left: parent.left
            leftMargin: 10
            bottom: parent.bottom
            bottomMargin: 25
        }

        onClicked: {
            Qt.quit()
        }
    }

    WaitIndicator {
        id: waitIndicator
        anchors.centerIn: aboutView
        show:  false
    }

    AuthenticationMgr {
        id: google

        onLoadingFinished: {
            console.log("AuthenticationMgr onLoadFinished");
            console.log("aRetStr is " + aRetStr);

//            MRApplication.controller.appInitialize(true);
            MRApplication.controller.appInitialize();
//            MRApplication.controller.opmlConfig().initUserFolder(userName.text);

            var retObject = eval('(' + aRetStr + ')')

            var index = 0;

            var url;

            while (index < retObject.subscriptions.length)
            {
                var mTitle = "";
                var mLink = "";
                var mCatName = "";

                mTitle = retObject.subscriptions[index].title;
                mLink = retObject.subscriptions[index].id;

                if(typeof(retObject.subscriptions[index].categories[0]) === "object")
                {
                    mCatName = retObject.subscriptions[index].categories[0].label;
                }
                google.insertMap(mCatName, mTitle, mLink, false);
                ++ index;
            }
            google.saveCachedUrl();
            google.composeOpml();

            application.userName = userName.text;
            application.password = password.text

            var userInfo = {
                userName: application.userName,
                password: application.password
            }

            application.getAccMgr().createUserInfo(userInfo)

            waitIndicator.show = false;            
            MRApplication.controller.opmlConfig().initUserFolder(userName.text);
            MRApplication.controller.opmlConfig().loadOpml();
            viewManager.getView(appState.kMainView).initialize();
            MRApplication.controller.handleCommand(opIdToMainView, viewManager.fade)
            application.cursorBarModelChanged();
        }

        onLoadingError: {
            console.log("google loading error");
            waitIndicator.show = false;
        }
    }

}
