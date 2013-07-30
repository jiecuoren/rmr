import QtQuick 1.0
import "../component"
import "../views"
import "../javascript/PageViewCreation.js" as PageCreater

Item {
    id: container

    property int currentViewID: 0
    property int maxPageNumber: 6

    function createNewPage(pageID){

        console.debug("pagemanager: try to create id:", pageID)
        var retIndex = -1

        if ( pagesModel.count >= maxPageNumber ){
            __delByRef()
        }

        PageCreater.createPageObjects(pageID)

        pagesModel.append({"item":PageCreater.pageViewItem, "id": pageID, "ref": 0})

        retIndex = pagesModel.count - 1

        if (null === viewSwitcher.currentView){
            viewSwitcher.currentView = PageCreater.pageViewItem
            var ref = pagesModel.get(retIndex).ref
            pagesModel.setProperty( retIndex, "ref", ref+1 )
        }

        return retIndex
    }

    function setActive(pageID){
        var toPage = __getByID(pageID)

        if (-1 === toPage){
            toPage = createNewPage(pageID)
         }

        viewSwitcher.switchView(pagesModel.get(toPage).item,
                                pagesModel.get(toPage).item.pageIndex < viewSwitcher.currentView.pageIndex,
                                0)

        // add ref count
        var ref = pagesModel.get(toPage).ref
        pagesModel.setProperty( toPage, "ref", ref+1 )

        viewSwitcher.currentView = pagesModel.get(toPage).item
        currentViewID = pageID
    }

    function getActive(){
        return pagesModel.get(__getByID(currentViewID)).item
    }

    function showSelf(flag){
        pagesModel.get(__getByID(currentViewID)).item.showSelf(flag)
    }


    function __getByID(pageID){
        var cnt = pagesModel.count

        for (var i=0; i<cnt; i++){
            if (pagesModel.get(i).id == pageID)
                return i
        }
        return -1
    }

    function __delByRef(){

        var delIndex = 0
        var refCnt = -1
        var count = pagesModel.count

        for (var i=0; i<count; i++){
            if ((pagesModel.get(i).id != currentViewID) && (refCnt > pagesModel.get(i).ref)){
                refCnt = pagesModel.get(i).ref
                delIndex = i
            }
        }

        pagesModel.get(delIndex).item.destroy()
        pagesModel.remove(delIndex)

        console.debug("pagemanager: delete page %d from the model", delIndex)
    }

    // View switcher component, handles the view switching and animation
    ViewSwitcher {
        id: viewSwitcher
        // Rooted in contentPane
        root: container
    }

    ListModel{
        id: pagesModel
    }

    Component.onCompleted: {
    }

}
