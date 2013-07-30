import QtQuick 1.0
import MReader 1.0

Item {
    id: container
    width: parent.width
    height: 120

    signal subItemClicked
    signal subItemActive(int preindex, int index)
    signal updateContent

    property alias model: subItemList.model
    property bool isDownloading: false
    property alias count: subItemList.count
    property alias currentIndex: subItemList.currentIndex

    property bool inited: false
    property bool updateEnabled: true


    Image {
        id: updateImg
        source: "../resources/icon/animated_arrow.png"
        anchors {
            left: parent.left
            leftMargin: 10
            verticalCenter: parent.verticalCenter
        }
        visible: false
        //playing: visible

        Behavior on rotation {
            NumberAnimation { duration: 300 }
        }
    }

   ListView {
        id: subItemList
        width: parent.width; height: appState.subItemHeight
        anchors.left: parent.left
        anchors.top:  parent.top
        orientation: ListView.Horizontal
        focus: true
        clip: true
        spacing: appState.subItemSpace
        cacheBuffer: 120

        property bool updateFlag: false

        onContentXChanged: {
            if (!updateEnabled)
                return;

            updateImg.visible = contentX < -40;

            if (-100 >= contentX ) {
                updateFlag = true;
                updateImg.rotation = 180;
            }

            if (0 == contentX && updateFlag) {
                container.updateContent();
                updateImg.rotation = 0;
            }
        }


        delegate: Rectangle {
            id: item
           width: appState.subItemHeight;
           height: appState.subItemHeight
           color: "black"

           Image {
               id: img
               asynchronous: true
               source: iconUrl
               anchors.fill: item
           }

           Rectangle {
               id: textBack
               width: item.width
               height: item.height/2
               anchors{ left: parent.left; bottom: parent.bottom}
               color: "black"
               opacity: 0.6

               Text {
                   id: text
                   clip: true
                   font.pixelSize: appState.subItemFontSize
                   font.bold: true
                   color: appState.subItemFontColor
                   text: title.substr(0, 140)
                   anchors {
                       left: parent.left; leftMargin: 3
                       right: parent.right; rightMargin: 3
                   }
                   wrapMode: Text.WordWrap
                   horizontalAlignment: Text.AlignLeft
               }

           }

           Rectangle {
               id: leftYellowLine
               width: 2
               height: parent.height
               anchors { right: parent.left; top: parent.top }
               visible: false;
               color: "yellow"
           }

           Rectangle {
               id: rightYellowLine
               width: 2
               height: parent.height
               anchors { right: parent.right; top: parent.top }
               visible: false;
               color: "yellow"
           }

           MouseArea {
               anchors.fill: parent
               onClicked:{
                   if(item.ListView.isCurrentItem) {
                       console.debug("click on the item:", subItemList.currentIndex);
                       container.subItemClicked();
                   }
                   else if(subItemList.interactive) {
                       var preindex = item.ListView.view.currentIndex;
                       item.ListView.view.currentIndex = index;
                       container.subItemClicked();
                       container.subItemActive(preindex, index)
                   }
               }
           }

            states: [
                State {
                    name: "noImage"
                    when:  "" == img.source
                    PropertyChanges {
                        target: textBack ; anchors.top: parent.top
                        anchors.bottom: undefined;
                        color: (0 === index%2) ? "gray" : "#999799"
                        opacity: 1
                    }

                    PropertyChanges { target: text ;anchors.topMargin: 5;}
                    PropertyChanges { target: leftYellowLine ;visible: true}
                    PropertyChanges { target: rightYellowLine ;visible: true}
                }
            ]

        } // delegate end

        Component {
            id: highlight
            Rectangle {
                width: subItemList.height; height: width
                color: "lightsteelblue"; radius: 5
                x: subItemList.currentItem.x
//                z: 100
                Behavior on x {
                    SpringAnimation {
                        spring: 1
                        damping: 0.2
                    }
                }
            }
        }

    }
}



