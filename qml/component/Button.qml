import Qt 4.7

Rectangle {
    id: container
    signal clicked
    property string text: "Button"
    property string image: ""
    property string image_press: ""
    property string image2: ""
    property string image2_press: ""
    property bool   borderflag: false
    property int imgWidth: 0

    width: imgWidth == 0 ? imageItem.width : imgWidth
    height: imgWidth == 0 ? imageItem.height : imgWidth

    color: "transparent"
    border.width : borderflag ? 1 : 0
    border.color : "#999999"
    radius : 4

//    gradient: Gradient {
//        GradientStop {
//            id: topGrad; position: 0.0
//            color: mr.pressed ? "#aaaaaa"  :  "transparent" }
//    }

    onImageChanged: {
        imageItem.source = image
    }

    function changeBtnVisible()
    {
        if (image2 != ""){
            imageItem.visible = !imageItem.visible
            imageItem2.visible = !imageItem2.visible
        }
    }

    MouseArea {
        id: mr
        anchors.fill: parent
        onClicked: {
            container.clicked()
            changeBtnVisible()
        }

        onPressed: {
            imageItem.source = container.image_press
            imageItem2.source = container.image2_press
        }
        onReleased: {
            imageItem.source = container.image
            imageItem2.source = container.image2
        }

        onMousePositionChanged: {
            if (containsMouse){
                imageItem.source = container.image_press
                imageItem2.source = container.image2_press
            }else{
                imageItem.source = container.image
                imageItem2.source = container.image2
            }

        }
    }

    Image {
        id: imageItem
        source: container.image
        Component.onCompleted: imgWidth !=0 ? anchors.verticalCenter = parent.verticalCenter : undefined
    }

    Image {
        id: imageItem2
        source: container.image2
        visible: false
        Component.onCompleted: imgWidth !=0 ? anchors.verticalCenter = parent.verticalCenter : undefined
    }

    Text {
        id: txtItem
        text: container.text
        anchors.bottom: container.bottom
        anchors.horizontalCenter: container.horizontalCenter
        font.family: "Courier"
        font.pointSize: 6
        color: "white"
    }

    states: [
         State {
             name: "onlyImage"
             when: container.text == "Button"
             PropertyChanges { target: txtItem; visible: false}
         },
         State {
             name: "onlyText"
             when: container.image == ""
             PropertyChanges { target: txtItem; anchors.verticalCenter: container.verticalCenter; font.pointSize: 8 }
         }
     ]

}
