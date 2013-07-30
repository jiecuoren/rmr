import QtQuick 1.0
import MReader 1.0

MrView {
    id: backContainer
    width: 360
    height: 640
//    z: 100
    viewId: appState.kImageViewerView

    signal back

    function onActive(msg)
    {
        console.log("Image viewer onActive");
        listview.model = msg;
    }

    Rectangle {
        id: container
        anchors.fill: backContainer
        color: "black"
//            opacity: 1
            scale: 0.0

            LoginButton {
                id: quitBtn
                width: 60
                height: 50
                anchors{bottom: container.bottom; left: parent.left}
                text: "Back"
                onClicked: {
                    console.log("Image viewer back clicked")
                    backContainer.visible = false;
                    MRApplication.controller.handleCommand(MRApplication.controller.opIdActiveDetailView);
                    MRApplication.controller.handleCommand(MRApplication.controller.opIdShowToolBar, {"flag": true});
                    MRApplication.controller.handleCommand(MRApplication.controller.opIdShowTitleBar, {"flag": true});
                }
            }

            ListView {
                id: listview
                orientation: ListView.Horizontal
                snapMode : ListView.SnapToItem
                flickDeceleration: 1500
                width: parent.width
                spacing: 10
                anchors{top: parent.top; bottom: quitBtn.top}
                clip: true
                cacheBuffer: 360
                delegate: ImageViewerItem {
                    width: listview.width
                    height: listview.height

                }
            }
    }

    SequentialAnimation {
        id: enlargerAnimation
        running: false

        PropertyAnimation {target: container; property: "scale";
            from: 0.0; to: 1.0; duration: 400; easing.type: Easing.InQuad }
    }


    Component.onCompleted: {
        enlargerAnimation.running = true;
    }


}
