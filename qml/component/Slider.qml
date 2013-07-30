import Qt 4.7

Item {
    id: slider_container
    width: 279; height: 39
    anchors.horizontalCenter: parent.horizontalCenter

    property variant value: 75
    property variant length: 230

    signal valueChanged(int value)

    Image {
        id: slider_bar
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        source: "../resources/icon/slider-bar.png"
        MouseArea {
            id: barmouse
            width: length; height: parent.height
            anchors.centerIn: parent
            onClicked: {
                slider_container.value = barmouse.mouseX;
                slider_container.valueChanged(slider_container.value)
            }
        }
    }

    Image {
        id: name
        x: 0
        y: 10
        source: "../resources/icon/slider-bar-blue1.png"
    }

    Image {
        id: slider_bar_blue
        x: 6; y: 10
        width: slider_handle.x - x
        fillMode:"Tile"
        source: "../resources/icon/slider-bar-blue.png"
    }

    Image {
        id: slider_handle
        anchors.verticalCenter: parent.verticalCenter
        source: "../resources/icon/slider-handle1.png"
        MouseArea {
            anchors.fill: parent
            drag.target: slider_handle
            drag.axis: "XAxis"
            drag.minimumX: 0
            drag.maximumX: length

            onMousePositionChanged: {
                slider_container.valueChanged(slider_container.value)
            }
        }        
    }
    Binding {
        target: slider_container
        property: "value"
        value: slider_handle.x
    }   

    Component.onCompleted:{
        slider_handle.x = slider_container.value;
    }
}
