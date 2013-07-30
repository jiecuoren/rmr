import QtQuick 1.0
import MReader 1.0
import "../component"

MrView {
    id: mainview
    viewId: appState.kMainView
    width: screenWidth
    height: screenHeight

    function initialize(){
        viewCategory.item.initialize()
    }

    function activeCategoryView(msg){
        var flag = msg
        viewCategory.item.showSelf(flag)
    }

    function activeDetailView(msg){
        viewDetail.item.active(msg.flag, msg.url, msg.index)
    }

    // This item will contain the views that we switch between
    Item {
        id: viewsContainer
        width: parent.width
        height: parent.height

        Rectangle {anchors.fill: parent; color: "white"}

        Loader{
            id: viewDetail
            source: "../views/DetailView.qml"
            anchors.fill: parent
        }

        Loader{
            id: viewCategory
            source: "../views/CategoryView.qml"
            anchors.fill: parent
        }
    }

    Component.onCompleted: {
        console.debug("appState.loading:", appState.loading)
    }

}
