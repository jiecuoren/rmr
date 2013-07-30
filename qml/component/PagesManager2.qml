import QtQuick 1.0
import "../component"
import "../views"
import "../javascript/PageViewCreation.js" as PageCreater

Item {
    id: container

    property bool isPage1: true

    function initialize(modelId) {
        page1.initViewRect(modelId)
        page1.opacity = 1.0
    }

    function setActive(modelId) {
        if (viewSwitcher.running || appState.isReading)
            return false;

        var toPage = __getNextView();
        toPage.initViewRect(modelId);

        viewSwitcher.switchView(toPage,
                                viewSwitcher.currentView.pageIndex > toPage.pageIndex,
                                0);


        viewSwitcher.currentView = toPage;

        return true;
    }

    function getActive(){
        return viewSwitcher.currentView
    }

    function showSelf(flag) {
        viewSwitcher.currentView.showSelf(flag)
    }


    function __getNextView() {
        return (isPage1 = !isPage1) ? page1: page2;
    }

    function __getCurrentView() {
        return (isPage1) ? page1 : page2;
    }

    // View switcher component, handles the view switching and animation
    ViewSwitcher {
        id: viewSwitcher
        // Rooted in contentPane
        root: container
        currentView: page1
    }

    PageView{ id: page1 }

    PageView{ id: page2 }

    Component.onCompleted: {
    }

}
