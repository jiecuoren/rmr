import QtQuick 1.0

Item {
    id: container

    property string fontName: "Helvetica"
    property int fontSize: 16
    property color fontColor: "black"
    property bool fontBold: false
    property string text: "NOT SET"
    property string bgImage: '../resources/icon/list_item.png'
    property string bgImageSelected: '../resources/icon/list_item.png'
    property string bgImagePressed: '../resources/icon/list_item.png'
    property string bgImageActive: '../resources/icon/list_item.png'
    property bool selected: false
    property bool selectable: false
    property int textIndent: 0
    property int childCount: 0
    property int iconIndent: 0
    property string icon: ""
    property double iconOpacity: 1

    signal clicked
    signal pressAndHold(int mouseX, int mouseY)

    width: 360
    height: 64
    clip: false

    onSelectedChanged: selected ?  state = 'selected' : state = ''

    Image {
        id: background
        source: bgImage
        width: parent.width
        anchors.fill: parent
        visible: false
    }

    Rectangle{
        id: backRect;
        width: parent.height
        height: parent.width
        rotation: -90.0
        anchors.centerIn: parent

        gradient: Gradient {
            GradientStop { position: 0.0; color: "steelblue" }
            GradientStop { position: 0.6; color: "black" }
        }
    }
    Image {
        id: iconId
        visible: icon.length > 0
        source: icon
        width: visible ? height: 0
        smooth: true
        opacity: container.iconOpacity
        anchors {
            left: parent.left
            top: parent.top
            margins: 0.2*parent.height
            verticalCenter: parent.verticalCenter
            leftMargin: 8 + iconIndent
        }

    }

    Text {
        id: itemText
        width: parent.width - 35
        anchors {
            left: iconId.right
            top: iconId.top
            topMargin: 4
            bottomMargin: 4
            rightMargin: 8
            verticalCenter: container.verticalCenter
        }
        font {
            family: container.fontName
            pixelSize: container.fontSize
            bold: container.fontBold
        }
        color: container.fontColor
        elide: Text.ElideRight
        text: ">> " + container.text + "    (" + childCount +")"
        verticalAlignment: Text.AlignVCenter
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: container.clicked();
        onReleased: selectable && !selected ? selected = true : selected = false
        onPressAndHold: container.pressAndHold(mouseX,mouseY)
    }

    states: [
        State {
            name: 'pressed'; when: mouseArea.pressed
            PropertyChanges { target: background; source: bgImagePressed }
        },
        State {
            name: 'selected'
            PropertyChanges { target: background; source: bgImageSelected }
        },
        State {
            name: 'active';
            PropertyChanges { target: background; source: bgImageActive; }
        }
    ]
}
