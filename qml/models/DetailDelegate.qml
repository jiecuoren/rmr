import QtQuick 1.0
import MReader 1.0
import "../component"

Component {
    id: detailDelegate

    Item {
        id: rootItem
        width: PathView.view.width - 20
        height: PathView.view.height

        signal clicked
        signal pressed

        property string offset: detailModel !== undefined ? detailModel.offset : 0
        property string htmlUrl: detailModel !== undefined ? detailModel.link : ''
        property variant imglist: detailModel !== undefined ? detailModel.imagelist : undefined

        property color __yellow: "#504207"
        property color __lightyellow: "#E0CD80"        

        // title
        Flickable {
            id: titleContent
            anchors { top: parent.top; topMargin: 8 }
            width: parent.width; height: Math.min(80, titleText.height)
            clip: true; z: 1
            contentWidth: parent.width; contentHeight: titleText.height
            Text {
                id: titleText
                width: parent.width
                text: detailModel !== undefined ? detailModel.title : ''
                font { pixelSize: 18; bold: true }
                color: rootItem.PathView.view.isCurrentItem ? "blue" : __yellow
                wrapMode: Text.WordWrap
            }
            MouseArea {
                anchors.fill: parent
                onClicked: rootItem.clicked()
                onPressed: rootItem.pressed()

            }
        }


        // split bar
        Rectangle {
            id: split1
            width: parent.width; height: 1
            anchors { top: titleContent.bottom; topMargin: 4 }
            color: __lightyellow
        }

        // pubdata
        Text {
            id: pubdataText
            anchors { top: split1.bottom; topMargin: 2 }
            text: detailModel !== undefined ? detailModel.pubDate : ''
            font.pixelSize: 14; color: __lightyellow
        }

        // pics
        ListImage {
            id: picsList
            z: 2
            anchors { top: pubdataText.bottom; topMargin: 4 }
            model: imglist;
            width: parent.width; height: count > 0 ? appState.detailPicsHeight: 0
        }


        // split bar
        Rectangle {
            id: split2
            width: parent.width;
            height: picsList.height > 0 ? 1 : 0
            anchors { top: picsList.bottom; topMargin: 4 }
            color: __lightyellow
        }

        // content
        Flickable {
            id: content
            anchors { top: split2.bottom; topMargin: 2 }
            width: parent.width; height: Math.min(parent.height-content.y, contentText.height + hrefText.height + 6)
            clip: true
            z: 1.0
            contentWidth: parent.width; contentHeight: contentText.height + hrefText.height + 6
            Text {
                id: contentText
                width: parent.width
                //text: contentStr
                text: detailModel !== undefined ? detailModel.description : ""
                z: 2.0
                font.pixelSize: 18; color: "black"
                wrapMode: Text.WordWrap
                //textFormat: Text.PlainText
                textFormat: Text.RichText
                onLinkActivated: {
                    console.log(link + " link activated")
                    Qt.openUrlExternally(link);
                }
            }

            Text {
                id: hrefText
                height: 30; z: 2.0
                visible: detailModel !== undefined
                anchors.top: contentText.bottom
                wrapMode: Text.WordWrap; textFormat: Text.RichText
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16; color: "blue"
                text: "<i><u><b>浏览原文...</b></u></i>"
                MouseArea {
                    anchors.fill: parent
                    onClicked: Qt.openUrlExternally(htmlUrl);
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: rootItem.clicked()
                onPressed: rootItem.pressed()
            }
        }
        ScrollBar {
                scrollArea: content; width: 6;
                anchors{
                    top: content.top
                    right: content.right
                    bottom: content.bottom
                }
        }

        onClicked: {
            rootItem.PathView.view.itemClicked();
        }

        onPressed: {
            console.debug("htmlUrl:", htmlUrl);
            rootItem.PathView.view.itemPressed();
        }

        onOffsetChanged: {
            content.contentY = 0;
        }

        MouseArea {
            id: md
            anchors.fill: parent            
            onClicked: rootItem.clicked()
            onPressed: rootItem.pressed()
        }

        Component.onCompleted: {
        }
    }
}
