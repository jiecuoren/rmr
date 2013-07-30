import QtQuick 1.0

Rectangle {
    id: contextMenu
    x: -360
    border.color: "gray"
    border.width: 6
    radius: 2

    width: 100
    height: listview.count * 50

    property int currentMouseX: -1
    property int currentMouseY: -1

    property alias model: listview.model

    signal itemIsSelected(int index)

    function computeMenuPos(viewHeight, viewWidth, headingHeight) {
        if(currentMouseX + width > viewWidth) {
            x = viewWidth - width
        } else {
            x = currentMouseX
        }

        if(headingHeight + currentMouseY + height > viewHeight) {
            y = viewHeight - height
        } else {
            y = headingHeight + currentMouseY
        }
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: "pink"
        opacity: 0.8
    }

    Connections {
        target: application
        onPageIndexChanged:  {
            console.log("context menu onPageIndexChanged");
            contextMenu.visible = false;
        }
    }

    ListView {
        id: listview
        width: parent.width
        height: parent.height
        interactive: false
        delegate: Item {
                id: item
                width: listview.width
                height: 50

                Rectangle {
                    id: itemBackground
                    anchors.fill: parent
                    color: "black"
                }

                MouseArea {
                    id: itemMouseArea
                    anchors.fill: parent

                    onClicked: {
                        contextMenu.itemIsSelected(index)
                     }
                }

                states: [
                    State {
                        name: "itemClicked"
                        when: itemMouseArea.pressed
                        PropertyChanges {
                            target: item
                            scale: 1.2
                        }
                        PropertyChanges {
                            target: itemBackground
                            color: "#3A8ACD"
                        }
                    }
                ]

                transitions: [
                    Transition {
                        reversible: true
                        PropertyAnimation { target:item; property: "scale"; to: 1; duration: 100 }
                    }
                ]

                Text {
                    id: text1
                    x:  10
                    width: 340
                    height: parent.height
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.verticalCenterOffset: 10
                    font.pixelSize: 22
                    color: "#ffffff"
                    smooth: true
                    text: primaryText

                    elide: Text.ElideRight

                }

                // lines between items
                Rectangle {
                    id: rectangleLine
                    x: 1
                    y: parent.height - 3
                    width: parent.width - 2
                    height: 2
                    color: "#aaaaaa"
                    opacity: 0.3
                }
            }
    }
}

