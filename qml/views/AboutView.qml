import QtQuick 1.0
import MReader 1.0

MrView {
    id: aboutView
    width: screenWidth
    height: screenHeight

    Rectangle{id:bk; anchors.fill: parent; color: "grey"}

    MouseArea {
        anchors.fill: parent
        onClicked: {
            dismissTimer.stop();
            aboutView.visible = false
        }
    }

    Text {
        id: name
        text: "About Dialog"
    }

    Timer {
        id: dismissTimer
        running: false
        repeat: false
        interval: 10000; //10 seconds
        onTriggered: {
            aboutView.visible = false
        }
    }
}
