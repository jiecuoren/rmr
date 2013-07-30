import QtQuick 1.0
import MReader 1.0
import "../component"
import "../models"

MrView {
    id: container
    anchors.fill: parent

    property string modelUrl: ""
    property bool showList: false
    property variant shareDialog

    function update(object){

    }

    function active(flag, url, index){
        console.debug("DetailView.qml: ", flag, url, index);
        if (!flag) {
            showList = false;
//            visible = false;
//            detailListView.cleanup();
            return;
        }
//        visible = true;
        summaryListView.currentIndex = index;
        showList = true;
        modelUrl = (modelUrl = "") ? "": url;
    }

    function updateDragModel() {
        console.debug("updateDragmodel in...")
        var itemCnt = summaryListView.count;
        var index = summaryListView.currentIndex;

        console.debug("DetailView, initModel, index: ", index);
        detailListView.updateModel(detailModel.data((index+itemCnt-1)%itemCnt),
                                 detailModel.data(index),
                                 detailModel.data((index+1)%itemCnt));

        detailListView.isHead = index === 0;
        detailListView.isEnd = index == itemCnt - 1;
        console.debug("updateDragmodel out...")
    }



    function createShareDialog()
    {
        console.log("page view createDialog");

        var component;
        component = Qt.createComponent("../component/ShareDialog.qml");

         if (component.status == Component.Ready) {
             shareDialog = component.createObject(container);
             shareDialog.text =  detailModel.data(summaryListView.currentIndex).title + "--" + detailModel.data(summaryListView.currentIndex).link
             shareDialog.shareFinished.connect(onShareFinished);
             shareDialog.shareError.connect(onShareError);
             shareDialog.cancelClicked.connect(onCancelClicked);

         }
    }

    function onShareFinished()
    {
        console.log("page view onShareFinished");
        shareDialog.shareFinished.disconnect(onShareFinished);
        shareDialog.shareError.disconnect(onShareError);
        shareDialog.cancelClicked.disconnect(onCancelClicked);
        shareDialog.destroy();

    }

    function onShareError()
    {
        console.log("page view onShareError");
        shareDialog.shareFinished.disconnect(onShareFinished);
        shareDialog.shareError.disconnect(onShareError);
        shareDialog.cancelClicked.disconnect(onCancelClicked);
        shareDialog.destroy();
    }

    function onCancelClicked()
    {
        console.log("page view onCancelClicked");
        shareDialog.shareFinished.disconnect(onShareFinished);
        shareDialog.shareError.disconnect(onShareError);
        shareDialog.cancelClicked.disconnect(onCancelClicked);
        shareDialog.destroy();
    }

    Rectangle {anchors.fill: parent; color: "white"}

    MRDetailModel {
        id: detailModel
    }

    DetailItemList {
        id: detailListView
        width: parent.width; height: parent.height

        function setRefresh() {
            refreshTimer.start();
        }

        onDragItemClicked: {
            showList = true;
            MRApplication.controller.handleCommand(MRApplication.controller.opIdShowToolBar, {"flag": true});
            MRApplication.controller.handleCommand(MRApplication.controller.opIdShowTitleBar,{"flag": true});
        }

        onDragItemPressed: {
            showList = false;
            MRApplication.controller.handleCommand(MRApplication.controller.opIdShowToolBar, {"flag": false});
            MRApplication.controller.handleCommand(MRApplication.controller.opIdShowTitleBar, {"flag": false});
        }

        onDragFlickStarted: {
            flickCnt++
            preIndex = currentIndex
        }

        onDragFlickEnded: {
            if (currentIndex == preIndex)
                flickCnt--;
        }

        onDragMovementStarted: {            
            flickCnt = 0;
            preIndex = currentIndex;
        }

        onDragMovementEnded: {

            var itemCnt = summaryListView.count;

            if ((currentIndex-preIndex+3)%3 == 1) {                
                summaryListView.currentIndex = (summaryListView.currentIndex+(++flickCnt))%itemCnt;
            }else if ((preIndex-currentIndex+3)%3 == 1) {
                summaryListView.currentIndex = (summaryListView.currentIndex-(++flickCnt)+itemCnt)%itemCnt;
            }else {
                return;
            }

            updateDragModel();
        }

        Timer {
            id: refreshTimer
            interval: 500
            onTriggered: updateDragModel()
        }
    }


    SubItemList {
        id: summaryListView
        updateEnabled: false
        opacity: 1 ? 1.0 : 0.5
        model: detailModel
        y: parent.height - appState.toolBarHeight - height;
        x: showList ? 0 : parent.width
//        z: 100.0

        onSubItemActive: {
            if (index > preindex){
                detailListView.setModel(2, detailModel.data(index))
                detailListView.pathView.incrementCurrentIndex();
            }else {
                detailListView.setModel(0, detailModel.data(index))
                detailListView.pathView.decrementCurrentIndex();
            }

            detailListView.setRefresh();
        }

        onUpdateContent: {
            detailModel.refresh();
        }

        Behavior on x {
            SequentialAnimation {
                NumberAnimation { duration: 500; easing.type: Easing.InOutQuad }
                ScriptAction { script:
                        if (showList)
                            updateDragModel();
                }
            }
        }
    }

    onModelUrlChanged: {
        if (modelUrl != ""){
            detailModel.cleanup();
            detailModel.setFeedSource(modelUrl, "");
            detailListView.initModel(detailModel.data(summaryListView.currentIndex));
        }
    }

    Connections {
        target: application
        onDetailViewMidBtnClicked: {
        console.log("Detail view onDetailViewMidBtnClicked");
        createShareDialog();

        }
    }

    Component.onCompleted: {
        console.debug("detail view:", width, height)        
    }
}
