import QtQuick 1.0
import MReader 1.0
import "../models"
import "../component"

Component {
     id: listViewDelegate

     Item {
         id: listContainer
         // Modify appearance from these properties
         property int itemHeight: appState.headerItemHeight
         property alias expandedItemCount: subItemList.count

         // Flag to indicate if this delegate is expanded
         property bool expanded: true
         property string expandedCategoryTitle: title

         x: 0; y: 0;
         clip: true
         width: parent.width
         height: headerItemRect.height + subItemsRect.height

         function update() {
             console.log("listViewDelegate update!")
             if(!subItemsRect.isUpdating)
             {
                 subItemsRect.isUpdating = true;
                 feedModel.refresh();
             }
         }

         MRSimpleModel{
             id:feedModel

             onRssLoadingStartted: {
                 console.log("MRSimpleModel onRssLoadingStartted ");
                 subItemsRect.isUpdating = true;
             }

             onRssLoadingStopped: {
                 console.log("MRSimpleModel onRssLoadingStopped ");
                 subItemsRect.isUpdating = false;
             }

             onRssLoadingError: {
                 console.log("MRSimpleModel onRssLoadingError " + aErrorStr);
                 subItemsRect.isUpdating = false;
             }

             onRssLoadingCancel: {
                 console.log("MRSimpleModel onRssLoadingCancel ");
                 subItemsRect.isUpdating = false;
             }

         }

         ListItem {
             id: headerItemRect
             x: 0; y: 0
             width: parent.width
             height: parent.itemHeight
             text: title
             childCount: subItemList.count
             fontName: appState.headerItemFontName
             fontSize: appState.headerItemFontSize
             fontColor: appState.headerItemFontColor
             fontBold: true

             onClicked: {
                 console.log("Clicked on " + title + " at " +index);
                 listView.currentIndex = index
             }

             onPressAndHold: {
                 console.log("List Item pressAndHold!");
                 application.pessedAtPageView(title, index, mouseX, mouseY);
             }


             AnimatedImage {
                 id: refresh
                 fillMode: "PreserveAspectFit"
                 height: parent.height
                 source: appState.refresh
                 smooth: true
                 scale: 2.5
                 playing: subItemsRect.isUpdating
                 anchors {
                     right: parent.right
                     verticalCenter: parent.verticalCenter
                     rightMargin: 5
                 }

                 MouseArea {
                     anchors.fill: parent
                     onClicked: {
                         console.log("resfresh clicked")
                         if(!subItemsRect.isUpdating)
                         {
                             subItemsRect.isUpdating = true;
                             feedModel.refresh();
                         }
                     }
                 }
             }
         }

         Rectangle {
             id: subItemsRect

             property bool isUpdating: false

             color: "#444444"
             y: headerItemRect.height
             width: container.width
             height: parent.expanded ? appState.subItemHeight + 4: 0
             clip: true
             opacity: 1

             Behavior on height {
                 // Animate subitem expansion. After the final height is reached,
                 // ensure that it is visible to the user.
                 SequentialAnimation {
                     NumberAnimation { duration: container.animationDuration; easing.type: Easing.InOutQuad }
                     ScriptAction { script: ListView.view.positionViewAtIndex(index, ListView.Center) }
                 }
             }

             SubItemList{
                 id: subItemList
                 model: feedModel
                 anchors.top: parent.top
                 onSubItemClicked: {
                     console.debug("page view on subitem clicked...")
                     listContainer.ListView.view.itemClicked(url, currentIndex)
                     listContainer.ListView.view.currentIndex = index
                 }

                 onUpdateContent: {
                     console.log("update content!!!!");
                     if(!subItemsRect.isUpdating)
                     {
                         subItemsRect.isUpdating = true;
                         feedModel.refresh();
                     }
                 }
             }

             Connections {
                  target: container
                  onUpdateAll: update()
              }

             Component.onCompleted: {
                 if(!subItemList.inited)
                 {
                     subItemList.inited = true;
                     feedModel.setFeedSource(url, localpath);
                 }
             }

         }// end of rectangle

     }// end of item
 }
