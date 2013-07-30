import QtQuick 1.0
import MReader 1.0
import "../models"
import "../component"

MrView {
    id: container
    width: parent.width
    height: parent.height
    opacity: 0.0
    Rectangle{id:bk; anchors.fill: parent; color: bkColor}

    property string pageName: "NO SET"
    property int pageIndex: -1
    property string pageType: "folder"
    property string pageUrl: ""
    property string pageLocalPath: ""
    property variant pageListModel

    property int animationDuration: 500
    property color bkColor: "lightgrey"

    property bool isRefreshPlaying: false

    property variant dialog

    property string holdItemTitle: ""
    property int holdItemIndex: -1

    property string dialogType: ""

    signal itemSelected(string title)
    signal itemLongTapped(string title, string url, int mouseX, int mouseY)
    signal discoveryClicked(string title)
    signal updateAll()

    function initViewRect(pageidx){

        showSelf(true);

        if (pageidx == container.pageIndex){
            return;
        }

        container.pageIndex = pageidx;

        var objc = MRApplication.controller.opmlConfig().getFolderListModel().data(pageidx);
        container.pageName = objc.title;
        container.pageType = objc.type;
        container.pageUrl = objc.url;
        container.pageLocalPath = objc.urlLocalPath;

        console.debug("page view, initialize...", container.pageName,container.pageType, container.pageUrl);
        if ( container.pageType == "folder" ){
            pageListModel = MRApplication.controller.opmlConfig().getFeedListByFolderName(pageName);
        }else if (pageType == "rss"){
            viewFeedDelegate.localpath = pageLocalPath;
            viewFeedDelegate.url = pageUrl;
            pageListModel = viewFeedDelegate.model;            
        }
        listView.model = pageListModel;
    }

    function showSelf(flag){
        container.y = flag ? 0: MRApplication.controller.height;
        appState.currentTitle = flag ? appState.appName : pageName;
    }

    function createDialog()
    {
        console.log("page view createDialog");

        var component;

        if("addMore" == dialogType)
        {
            component = Qt.createComponent("../component/Dialog.qml");
        }
        else
        {
            component = Qt.createComponent("../component/RenameDialog.qml");
        }

         if (component.status == Component.Ready) {
             dialog = component.createObject(container);
             dialog.yesClicked.connect(dialogYesClicked);
             dialog.noClicked.connect(dialogNoClicked);
         }
    }

    function dialogYesClicked(type)
    {
        console.log("page view dialogYesClicked " + dialog.feedAddress + "   " + dialog.feedName);
        dialog.yesClicked.disconnect(dialogYesClicked);
        dialog.noClicked.disconnect(dialogNoClicked);
        dialog.destroy();

        if("addMore" == dialogType)
        {
            MRApplication.controller.opmlConfig().addItemByUser(dialog.feedAddress, dialog.feedName, pageName)
        }
        else
        {
            MRApplication.controller.opmlConfig().renameItemByUser(holdItemTitle, dialog.renameText, pageName, holdItemIndex)
        }
    }

    function dialogNoClicked()
    {
        console.log("page view dialogNoClicked");
        dialog.yesClicked.disconnect(dialogYesClicked);
        dialog.noClicked.disconnect(dialogNoClicked);
        dialog.destroy();
    }

    function pressAndHoldAtView(title, index)
    {
        console.log("pressAndHoldAtView(title, index)");
        holdItemTitle = title
        holdItemIndex = index
    }

    Behavior on y {
        // Animate subitem expansion. After the final height is reached,
        // ensure that it is visible to the user.

        SequentialAnimation {
            NumberAnimation { duration: container.animationDuration; easing.type: Easing.InOutQuad }
            ScriptAction { script:
                    if (container.y == 0) {
                        MRApplication.controller.handleCommand(MRApplication.controller.opIdActiveDetailView,
                                                               {"flag": false, "model": undefined, "url": undefined, "index": undefined})
                    }else {
                        MRApplication.controller.handleCommand(MRApplication.controller.opIdActiveDetailView,
                                                                          listView.msg);
                        // should be removed later
                        MRApplication.controller.handleCommand(MRApplication.controller.opIdShowToolBar, {"flag": true});
                    }
              }
        }


    }

    Image {
        id: updateAllImg
        source: "../resources/icon/arrow_right.png"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top:  parent.top
        visible: false
        Behavior on rotation {
            NumberAnimation { duration: 300 }
        }
    }

    Text {
        id: updateText
        text: "Go on draging to update all items!"
        width: parent.width
        anchors.top: updateAllImg.bottom
        anchors.topMargin: 10
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 20
        color: "black"
        wrapMode: Text.WordWrap
        visible: false
    }

    ListView {
        id: listView
        clip: true
        visible: true
        anchors.fill: container
        cacheBuffer: MRApplication.controller.height*2
        delegate: pageType == "folder" ? viewFolderDelegate : viewFeedDelegate.delegate
        focus: true
        spacing: 0

        footer: Rectangle {
            id: footRec
            width: 150
            height: 50
            color: "gray"
            radius: 5
            visible: pageType == "folder"
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                text: "+"
                font {pixelSize: 35; bold: true}
                color: "white"
                anchors.centerIn: parent
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log("add more clicked!")
                    dialogType = "addMore"
                    createDialog();
                }

                onPressAndHold: {
                    console.log("page view long pressed")
                }
            }

        }

        property bool updateEnabled: false
        property variant msg
        property bool beyondViewEnd: false

        function itemClicked(url, index) {
            appState.isReading = true;
            MRApplication.cancelAllRssImageRequest();

            MRApplication.controller.handleCommand(MRApplication.controller.opIdActiveCategoryView,
                                                   false);
//            MRApplication.controller.handleCommand(MRApplication.controller.opIdActiveDetailView,
//                                                   {"flag": true, "url": url, "index": index});
            msg = {"flag": true, "url": url, "index": index};
        }

        onBeyondViewEndChanged: {
            MRApplication.controller.handleCommand(MRApplication.controller.opIdShowToolBar, {"flag": !beyondViewEnd});
        }
         onContentYChanged: {

             if(-60 >= listView.contentY) {

                updateAllImg.visible = true;
                updateText.visible = true;
            }

            if(-60 <= listView.contentY) {
                updateAllImg.visible = false;
                updateText.visible = false;
                updateAllImg.rotation = 90;
            }

            if ((-200 >= listView.contentY))
            {
                updateText.text = "Release to update all items!"
                updateEnabled = true;
                updateAllImg.rotation = 270;
            }

            if (0 == listView.contentY && updateEnabled) {
                console.log("0 == listView.contentY in page view");                
                updateText.text = "Go on draging to update all items!"

                if (pageType == "folder") {
                    updateAll();
                }else if (pageType == "rss") {
                    viewFeedDelegate.update();
                }
                //we don't know if all the horizontal view finished update, just emit the signal
                // subitemlist know should update or not.
                updateEnabled = false;
            }

            beyondViewEnd = contentY >= contentHeight - height && atYEnd;
        }

         onCountChanged: {
             if (count > 0) {
                 appState.loading = false;
             }else if (count == 0 && pageType == "rss") {
                 appState.loading = true;
             }
         }
    }

    PageFolderDelegate { id: viewFolderDelegate }
    PageFeedDelegate {id: viewFeedDelegate }

    Connections {
        target: application
        onPessedAtPageView:  {

            if(contextMenu.visible == false) {
                pressAndHoldAtView(title, index)
                contextMenu.currentMouseX = mouseX
                contextMenu.currentMouseY = mouseY + index * 100
                console.log("onPessedAtPageView  " + index + "--" + contextMenu.currentMouseX + "--" + contextMenu.currentMouseY)
                contextMenu.visible = true
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onPressed: {
            console.log("pressed on pageview view")
            if(contextMenu.visible) {
                contextMenu.visible = false
            }
            else {
                mouse.accepted = false
            }
        }
    }

    ContextMenu {
        id: contextMenu;
        visible: false;
        model: contextModel

        onVisibleChanged: {
            if(visible) {
                computeMenuPos(container.height, container.width, 0)
            }
        }

        ListModel {
            id: contextModel
            ListElement{primaryText: "Rename"}
            ListElement{primaryText: "Delete"}
        }

        onItemIsSelected: {
            console.log("onItemIsSelected")
            contextMenu.visible = false
            if(0 === index) {
                console.log("contextMenu index is 0 ")
                dialogType = "Rename"
                createDialog();
            } else if (1 == index) {
                console.log("contextMenu index is 1")
                MRApplication.controller.opmlConfig().deleteItemByUser(holdItemTitle, pageName, holdItemIndex)
            }

        }
    }


}
