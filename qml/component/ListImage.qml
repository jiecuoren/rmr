import QtQuick 1.0

Item {
    id: container

    signal longPressOnImg

    property alias model: picsList.model
    property alias count: picsList.count

    ListView {
        id: picsList
        anchors.fill: parent
        delegate: listDelegate
        orientation: ListView.Horizontal
        clip: true; spacing: 5
        currentIndex: 0
    }

    Component {
        id: listDelegate
        Item {
            id: rootItem
            width: appState.detailPicsHeight; height: width
            Rectangle {anchors.fill: parent; color: "black"}

            property string refreshicon: "../resources/icon/rotation_icon.png"

            Image {
                id: image
                asynchronous: true
                source: localpath
            }

            MouseArea {
                id: md
                anchors.fill: parent

                onClicked: {
                    rootItem.ListView.view.currentIndex = index;
                    console.debug("list image clicked, local path:", localpath, index);
                }

                onPressAndHold: {
                    console.log("container on long press and hold");
                    container.longPressOnImg()
                }
            }

            Component.onCompleted: {
                console.debug("list image, local path:", localpath, index);
                rootItem.ListView.view.model.downloadImage(index);
            }
        }
    }
}
