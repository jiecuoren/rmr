import QtQuick 1.0
import MReader 1.0
import "./component"
import "./models"
import "./views"
import "./javascript/accountManager.js" as AccMgr

MRApplication {
    id: application
    width: screenWidth
    height: screenHeight

    property int opIdInitialize: 0
    property int opIdToMainView: 1
    property int opIdShowToolBar: 2
    property int opIdShowTitleBar: 3
    property int opIdActiveDetailView: 4
    property int opIdActiveCategoryView: 5
    
    signal pessedAtPageView(string title, int index, int mouseX, int mouseY)
    signal pageIndexChanged();

    signal cursorBarModelChanged();

    signal detailViewMidBtnClicked();

    property string userName: ""
    property string password: ""

    function handleCommand(command, msg){
        console.debug("controller handle Conmmand: ", command)
        switch(command){
        case opIdInitialize:
            console.debug("controller handle Conmmand: case opIdInitialize: ")
            appInitialize();
            viewManager.getView(appState.kMainView).initialize();
            break;
        case opIdActiveDetailView:
            viewManager.getView(appState.kMainView).activeDetailView(msg);
            break;
        case opIdActiveCategoryView:
            viewManager.getView(appState.kMainView).activeCategoryView(msg);
            toolBar.updateShowingBtn(!msg);
            break;
        case opIdToMainView:
            viewManager.activeView(appState.kMainView, msg);
            break;
        case opIdShowToolBar:
            showToolBar(msg.flag);
            break;
        case opIdShowTitleBar:
            showTitleBar(msg.flag)
            break;
        default:
            console.debug("invalid command id")
            break;
        }
    }

    function showToolBar(flag){
        toolBar.showing = flag;
    }

    function showTitleBar(flag){
        titleBar.showing = flag;
    }

    //change below function to create your own view
    function createView(viewId)
    {
        console.log("main.qml, createView(), viewId = " + viewId);
        switch (viewId)
        {
        case appState.kSplashView:
            splashViewLoader.source = "./views/SplashView.qml"
            return splashViewLoader.item;

        case appState.kMainView:
            mainViewLoader.source = "./views/MainView.qml"
            return mainViewLoader.item;

        case appState.kLoginView:
            loginViewLoader.source = "./views/LoginView.qml"
            return loginViewLoader.item;

        default:
            break;
        }
    }

    function getAccMgr()
    {
        return AccMgr;
    }

    function getCursorModel()
    {
        console.log("main.qml getCursorModel");
        return MRApplication.controller.opmlConfig().getFolderListModel();
    }

    function deleteMainviewModel()
    {
        console.log("main.qml deleteMainviewModel");
        mainViewLoader.sourceComponent = undefined;
    }

    Loader {id: splashViewLoader; anchors.fill: application; z: 100}
    Loader {id: mainViewLoader; width: parent.width; anchors{top: titleBar.bottom; bottom: parent.bottom}}
    Loader {id: loginViewLoader; width: parent.width; anchors{top: titleBar.bottom; bottom: parent.bottom}}

    MrViewManager{id: viewManager; anchors.fill: application}


    TitleBar{
        id: titleBar
        anchors.top: parent.top
    }

    ToolBar{
        id: toolBar
        anchors.bottom: parent.bottom
        showing: viewManager.currentViewID() == appState.kMainView

        onSettingClicked:{
            if ( showingBackBtn ){
                appState.isReading = false
                MRApplication.controller.handleCommand(MRApplication.controller.opIdActiveCategoryView,
                                                       true)
            }else{
                console.debug("setting button clicked...");
//                optionMenu.visible = !optionMenu.visible;
            }
        }

        onMidBtnClicked: {
            if ( showingBackBtn ){
                console.log("onMidBtnClicked");
                detailViewMidBtnClicked();
            }
        }       
    }

    Loader {
        id: visual
        property alias theme: visual.item
        source: "./component/Theme.qml"
    }

    // Properties.
    AppStateVars { id: appState }

    // Wait indicator is also not visible by default, only when appState.loading === true
    WaitIndicator {
        id: waitIndicator
        z: 100
        anchors.centerIn: application
        show: appState.loading
    }

    OptionMessageBox {
        id: warningBox
        anchors.fill: parent
        visible: false
        text: ""

        property int type: -1

        onYesClicked: {
            console.log("OptionMessageBox onYesClicked");
            if(0 == type)
            {
                waitIndicator.show = true;
                google.doLogin(userName, password, false);
            }
//            else if(2 == type)
//            {
//                MRApplication.cancelAllRssImageRequest();
////                appInitialize(true);
//                deleteMainviewModel();
//                viewManager.activeView(appState.kLoginView, viewManager.fade)
//                application.getAccMgr().clearDatabase();
//                google.clareCacheFile();
//            }
            warningBox.visible = false;
        }

        onNoClicked: {
            console.log("OptionMessageBox onNoClicked");
            warningBox.visible = false;
        }
    }

    ContextMenu {
        id: optionMenu;
        visible: false;
        width: 250
        model: optionModel
        anchors.bottom: toolBar.top
        anchors.left: parent.left

        ListModel {
            id: optionModel
            ListElement{primaryText: "Sync google account"}
//            ListElement{primaryText: "Add page"}
//            ListElement{primaryText: "Log off"}
        }

        onItemIsSelected: {
            console.log("optionModel onItemIsSelected")
            optionMenu.visible = false
            warningBox.type = index
            if(0 === index)
            {
                console.log("optionModel index is 0 ")
                warningBox.text = "are you sure sync google?"
                warningBox.visible = true;
            }
//            else if(1 === index)
//            {
//                console.log("optionModel index is 1 ")

//            }
//            else if(2 === index)
//            {
//                console.log("optionModel index is 2 ")
//                warningBox.text = "are you sure log off?"
//                warningBox.visible = true;

//            }

        }

    }

    AuthenticationMgr {
        id: google

        onLoadingFinished: {
            console.log("AuthenticationMgr onLoadFinished in main");
            console.log("aRetStr is " + aRetStr);

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
                // we just give all link to the same cat name,
                if(typeof(retObject.subscriptions[index].categories[0]) === "object")
                {
                    mCatName = retObject.subscriptions[index].categories[0].label;
                }
                google.insertMap(mCatName, mTitle, mLink, true);
                ++ index;
            }

            google.syncOpml();
            google.saveCachedUrl();
            appInitialize();
            MRApplication.controller.opmlConfig().initUserFolder(userName);
            MRApplication.controller.opmlConfig().loadOpml();
            waitIndicator.show  = false;
            cursorBarModelChanged();

        }

        onLoadingError: {
            console.log("google loading error");
            waitIndicator.show = false;
        }
    }


    Component.onCompleted: {        
        viewManager.activeView(appState.kSplashView, viewManager.noAnimation)
    }
}
