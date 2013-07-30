import QtQuick 1.0
import Qt.labs.gestures 2.0


Rectangle {
    id: bk
    width: 360
    height: 640

    Text {
        id: test
        text: "test!"
        font.pixelSize: 50
    }

    GestureArea {
        id: ges
        anchors.fill: parent
        onTap:{
            console.log("Yes!!!!");
//            test.text = "yes!!!!!"
        }

        onSwipe: {
            test.text = "Yes, swipe angle is" + gesture.swipeAngle
//            test.text = "yes!!!!!"
        }
    }
}

