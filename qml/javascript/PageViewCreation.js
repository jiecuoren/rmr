var component;
var pageViewItem;

function createPageObjects(pageidx) {

    component = Qt.createComponent("../views/PageView.qml");
    if (component.status == Component.Ready)
    {
        finishCreation(pageidx);

    }
    else if (component.status == Component.Error)
    {        
        console.log("component Error:", component.errorString());
    }
    else{
        console.log("page creator status change cause finish")
        component.statusChanged.connect(finishCreation);
    }
}

function finishCreation(pageidx) {
    if (component.status == Component.Ready) {
        pageViewItem = component.createObject(container);
        if (pageViewItem == null) {
            console.log("component createObject failed");
        } else {
            pageViewItem.initViewRect(pageidx);
        }
    }
    else if (component.status == Component.Error) {
        console.log("Error loading component:", component.errorString());
    }
}
