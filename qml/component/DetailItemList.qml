import QtQuick 1.0
import "../models"

Item {
    id: container

    signal dragItemClicked
    signal dragItemPressed
    signal dragFlickStarted
    signal dragFlickEnded
    signal dragMovementStarted
    signal dragMovementEnded

    property alias pathView: itemListView
    property alias model: itemListView.model
    property alias interactive: itemListView.interactive

    property int flickCnt: 0
    property bool isHead: false
    property bool isEnd: false
    property alias currentIndex: itemListView.currentIndex
    property alias preIndex: itemListView.preIndex
    property alias highlightMoveDuration: itemListView.highlightMoveDuration

    function cleanup()
    {
        updateModel(undefined, undefined, undefined);
    }

    function setModel(index, object)
    {
        // index range: 0 - left, 1, 2 - right,
        var toIdx = 0;

        switch (index){
        case 0:
            toIdx = (currentIndex+2)%3;
            break;
        case 1:
            toIdx = currentIndex%3;
            break;
        case 2:
            toIdx = (currentIndex+1)%3;
            break;
        default:
            break;
        }
        if (dragModel.model.get(toIdx) === undefined ||
                object != dragModel.model.get(toIdx).detailModel)
            dragModel.model.setProperty(toIdx, "detailModel", object);
    }

    function initModel(object1) {
        setModel(1, object1);
    }

    function updateModel(object0, object1, object2) {
        // object1 - current; object0 - left; object2 - right

        console.debug(object0, object1, object2, dragModel.model.count, currentIndex);

        if (dragModel.model.count === 0) {
            dragModel.model.append({"detailModel": object1});
            dragModel.model.append({"detailModel": object2});
            dragModel.model.append({"detailModel": object0});
        }else {
            setModel(1, object1);
            setModel(0, object0);            
            setModel(2, object2);
        }
    }

//    onIsEndChanged: itemListView.interactive = !isEnd
//    onIsHeadChanged: itemListView.interactive = !isHead

    PathView{
        id: itemListView
        anchors.fill: parent
        pathItemCount: 3
        preferredHighlightBegin: 0.5
        preferredHighlightEnd: 0.5
        model: dragModel.model
        flickDeceleration: 1000
        interactive: true
        currentIndex: 0
        delegate: DetailDelegate {}

        property int preIndex: currentIndex

        path: Path {
            startX: -parent.width; startY: parent.height/2
            PathLine { x: parent.width*2; y: parent.height/2; }
        }

        function itemClicked() {
            container.dragItemClicked();
        }

        function itemPressed() {
            container.dragItemPressed();
        }

        function dragingArea(movingRight) {

            if (!isEnd && !isHead)
                return;

            interactive = !((movingRight && isEnd) || (!movingRight && isHead));

            console.debug("dragingArea, movingRight: ", movingRight, isEnd, isHead, interactive);
        }

        onFlickStarted: {
            console.debug("detailItemList, onFlickStart");
            //container.dragFlickStarted();
        }

        onFlickEnded: {
            console.debug("detailItemList, onFlickEnded");
            //container.dragFlickEnded();
        }

        onMovementStarted: {
            console.debug("detailItemList, onMovementStarted");
            container.dragMovementStarted();
        }

        onMovementEnded: {
            console.debug("detailItemList, onMovementEnded: ", currentIndex);
            container.dragMovementEnded();
        }

        onMovingChanged: {
            if (moving) {
                console.debug("detailItemList, moving started");

            }else {
                console.debug("detailItemList, moving ended");
            }
        }

        Component.onCompleted: {
        }

    }

    DetailDragModel {
        id: dragModel
    }
}
