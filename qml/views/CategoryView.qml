import QtQuick 1.0
import MReader 1.0
import "../component"


MrView {
    id: container
    anchors.fill: parent

    function initialize(){
        pageManager.initialize(cursorBar.currentIdx)
    }

    function showSelf(flag){
        console.debug("CategoryView showself in:", flag);
        cursorBar.showSelf(flag)
        pageManager.showSelf(flag)
        console.debug("CategoryView showself out");
    }

    CursorBar{
        id: cursorBar

        onIndexChange: {
            MRApplication.cancelAllRssImageRequest();
            application.pageIndexChanged();
            if (!pageManager.setActive(cursorBar.currentIdx)) {
                backToPreIndex();
            }
        }
    }

    PagesManager2{
        id: pageManager
        width: parent.width
        anchors{
            top: cursorBar.bottom
            bottom: parent.bottom
        }
    }

    Component.onCompleted: {

    }
}
