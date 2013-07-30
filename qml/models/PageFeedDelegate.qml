import QtQuick 1.0
import MReader 1.0
import "../models"
import "../component"

Item {
     id: listViewDelegate
     anchors.fill: parent

     property alias model: feedModel
     property alias delegate: feedDelegate
     property alias url: feedModel.url
     property alias localpath: feedModel.localpath

     function update() {
         if(!feedModel.isUpdating)
         {
             feedModel.isUpdating = true;
             feedModel.refresh();
         }
     }

     Component{
         id: feedDelegate

         Rectangle {
             id: rootItem
             width: parent.width
             height: itemRow.height
             color: index&1 ? "white" : "lightgrey"

             Row{
                 id: itemRow
                 property int itemWidth: parent.width
                 property int contentHeight: 80
                 property int itemSpace: 5

                 x: itemSpace
                 width: parent.width - itemSpace*3
                 height: Math.max(row1.height, row2.height) + itemSpace*4
                 spacing: itemSpace
                 Column{
                     id: row1
                     anchors  {
                         verticalCenter: parent.verticalCenter
                     }
                     width: parent.width - row2.width
                     spacing: 8

                     Text {
                         id: itemTitle
                         elide: Text.ElideRight
                         width: parent.width
                         font{ pixelSize: 16; bold: true }
                         color: "steelblue"
                         text: title
                     }
                     Text {
                         id: itemText
                         clip: true
                         height: font.pixelSize * 3 + 6
                         width: parent.width
                         font{ pixelSize: 15 }
                         text: excerpt
                         textFormat: Text.RichText; wrapMode: Text.WordWrap
                     }

                     Text {
                         id: itemPub
                         width: parent.width
                         font{ pixelSize: 14; bold: true }
                         text: pubDate
                     }


                 }

                 Image {
                     id: row2
                     width: iconUrl === "" ? 0 : 120
                     height: iconUrl === "" ? 0 : 120
                     anchors  {verticalCenter: parent.verticalCenter}
                     source: iconUrl === "" ? "" : iconUrl
                 }
             }

             MouseArea {
                 anchors.fill: parent

                 onClicked: {
                     console.log("PageFeedDelegate Clicked on " + title + " at " +index)
                     rootItem.ListView.view.itemClicked(rootItem.ListView.view.model.url, index)
                 }
             }

         }

  }

     MRSimpleModel{
         id:feedModel
         property string url: ""
         property string localpath: ""
         property bool isUpdating: false

         onRssLoadingStartted: {
             console.log("MRSimpleModel onRssLoadingStartted ");
             isUpdating = true;
         }

         onRssLoadingStopped: {
             console.log("MRSimpleModel onRssLoadingStopped ");
             isUpdating = false;
         }

         onRssLoadingError: {
             console.log("MRSimpleModel onRssLoadingError " + aErrorStr);
             isUpdating = false;
         }

         onRssLoadingCancel: {
             console.log("MRSimpleModel onRssLoadingCancel ");
             isUpdating = false;
         }


     }

     onUrlChanged: {
         feedModel.setFeedSource(url, localpath);
     }

     Component.onCompleted: {
     }
}
