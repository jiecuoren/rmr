import QtQuick 1.0

Rectangle {
    id: button
    color: "gray"

    radius: 5
    border.width: 3
    border.color: "white"

    property alias text: btnText.text

    signal clicked

    Text {
        id: btnText
        anchors.centerIn: parent
        font.pixelSize: 20
        color: "white"
    }

    MouseArea {
        anchors.fill: parent
        onPressed: {
            button.color = "red";
        }

        onPositionChanged: {
            if (containsMouse)
            {
                button.color = "red";
            }
            else
            {
                button.color = "gray";
            }
        }

        onReleased: {
            button.color = "gray";
            if (containsMouse)
            {
                button.clicked();
            }
        }
    }
}
