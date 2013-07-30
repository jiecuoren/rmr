import QtQuick 1.0
import MReader 1.0
import "../component"

Rectangle {
    id: container
    width: parent.width
    height: barHeight
    color: "#606060"

    property int barHeight: appState.cursorBarHeight
    property int gapHeight: 4
    property int itemWidth: 90
    property int itemMoveDuration: 200
    property int currentIdx:  0
    property alias model: pageList.model

    signal indexChange
    property int preindex: pageList.currentIndex

    function showSelf(flag){
        container.y = flag ? 0 : -barHeight
    }

    function backToPreIndex() {
        pageList.currentIndex = preindex;
    }

    Binding {target: container; property: "currentIdx"; value: pageList.currentIndex}

    Component {
        id: highlight       
        Rectangle {
            width: itemWidth; height: pageList.height + 1
            color: "black"
            Rectangle{ id: top; width: parent.width; height: 1; color: "#E0CD80" }
            Rectangle{ id: left; width: 1; height: parent.height; color: "#E0CD80" }
            Rectangle{
                id: right
                width: 1; height: parent.height
                anchors.right: parent.right
                color: "#E0CD80"
            }
        }
    }

    Behavior on y {
        // Animate subitem expansion. After the final height is reached,
        // ensure that it is visible to the user.
        ParallelAnimation {
            NumberAnimation { duration: container.itemMoveDuration*2; easing.type: Easing.InOutQuad }
        }
    }


    ListView {
        id: pageList
        width: parent.width; z: 100
        anchors{ top: parent.top; bottom: bottomGap.top }
        orientation: ListView.Horizontal
        //highlightFollowsCurrentItem: true
        highlightMoveDuration: itemMoveDuration
        model: MRApplication.controller.opmlConfig().getFolderListModel()
        delegate: pagesDelegate
        highlight: highlight
        focus: true
        spacing: 0        
        visible: container.height == 0 ? false: true

        Component {
            id: pagesDelegate

            Item {
                //width: appState.cursorItemWidth;
                width: itemWidth
                height: pageList.height
                clip: true
                Text {
                    id: titleInfo
                    property string titlestr: title

                    anchors { fill: parent; margins: 5 }

                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: titlestr.toUpperCase()
                    color: "white"
                    //elide: Text.ElideRight
                    wrapMode: Text.WordWrap

                    font{
                        bold: ListView.isCurrentItem
                        pixelSize: ListView.isCurrentItem ? 16 : 16
                    }
                }

                Rectangle {
                    anchors.right: parent.right
                    width: 1; height:  parent.height
                    color: "#D8D8D8"
                }

                Rectangle {
                    id: triangle
                    x: parent.width - width/2; y: -height/2
                    width: 12; height: 12
                    rotation: 45
                    color: "#D8D8D8"
                    visible: type == "rss"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: itemClicked()
                }

                function itemClicked()
                {
                    if(ListView.isCurrentItem) {
                        console.debug("click on the item:", pageList.currentIndex)
                    }
                    else if(pageList.interactive) {
                        preindex = pageList.currentIndex;
                        pageList.currentIndex = index;
                        container.indexChange();
                    }
                }
            }
        }
    }

    Rectangle{
        id: bottomGap
        width: parent.width; height: gapHeight
        anchors.bottom: parent.bottom
        color: "black"
        Rectangle{
            id: bottom
            anchors.top: parent.top
            width: parent.width; height: 1
            color: "#E0CD80"
        }        
        MouseArea {anchors.fill: parent}
    }

    Connections {
        target: application
        onCursorBarModelChanged: {
        console.log("Cursor bar onCursorBarModelChanged");
            pageList.model = application.getCursorModel();
//                    MRApplication.controller.opmlConfig().getFolderListModel();
        }
    }
}
