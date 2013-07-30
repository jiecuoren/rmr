import QtQuick 1.0

Item {
    id: container

    property alias imageScale: image.scale
    property double imageZoomScale
    property double imageRealWidth
    property double imageRealHeight

    onImageScaleChanged: {
        console.log("onImageScaleChanged");
    }

    function calImageSize()
    {
        console.log("calImageSize");
        //use height zoom scale
        if(image.sourceSize.height > image.sourceSize.width)
        {
            imageZoomScale = imageBk.height/image.sourceSize.height
        }
        else
        {
            imageZoomScale = imageBk.width/image.sourceSize.width
        }
        imageRealWidth = image.sourceSize.width * imageZoomScale
        imageRealHeight =  image.sourceSize.height * imageZoomScale
    }

    Rectangle {
        id: imageBk
        width: parent.width
        height: parent.height - 100
        color: "transparent"

        Image {
            id: image
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: localpath/*imageurl*/

            onStatusChanged: {

                if (image.status == Image.Ready)
                {
                    calImageSize();
                }
            }
        }
    }

    MouseArea {
            id: ma
            anchors.fill: imageBk
            drag.target : imageBk
            drag.axis: Drag.XandYAxis
            drag.minimumX: 0
            drag.maximumX: 0
            drag.minimumY: 0
            drag.maximumY: 0

            onPressed: {
                if(1 == image.scale)
                {
                    imageBk.x = 0;
                    imageBk.y = 0;
                    mouse.accepted = false;
                }
            }
    }

    Slider {
        id: slider
        z: ma.z + 1
        anchors {bottom: parent.bottom; bottomMargin: 20; horizontalCenter: parent.horizontalCenter}
        onValueChanged: {

            image.scale = (230+value)/230
            if(0 == value)
            {
                ma.drag.minimumX = 0;
                ma.drag.maximumX = 0;
                ma.drag.minimumY = 0;
                ma.drag.maximumY = 0;
            }
            else
            {
                ma.drag.minimumX = -imageRealWidth*scale
                ma.drag.maximumX = imageRealWidth*scale
                ma.drag.minimumY = -imageRealHeight*scale
                ma.drag.maximumY = imageRealHeight*scale
            }

        }
    }

    Component.onCompleted: {
        console.log("image scale is: " + image.scale);
        calImageSize();
        container.ListView.view.model.downloadImage(index);
    }
}
