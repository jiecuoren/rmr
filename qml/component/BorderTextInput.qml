import QtQuick 1.0

Rectangle {
    id: borderTextInput

    property alias text: textInput.text
    property alias echoMode: textInput.echoMode
    property alias inputFocus: textInput.focus
    property alias textInputSize: textInput.font.pixelSize

    BorderImage {
        id: textBorderimage

        anchors.fill: parent

        source: "../resources/icon/editborder.png"
        border { left: 1; top: 1; right: 1; bottom: 1 }
    }

    TextInput {
        id: textInput

        anchors.fill: textBorderimage
        anchors {
            leftMargin: 5;
            rightMargin: 5;
            topMargin:5;
            bottomMargin: 5
        }

        activeFocusOnPress: true
        selectByMouse: true
        font.family: "Nokia Sans SemiBold"
        font.pixelSize: 16
        color: "#000000"
    }
}
