import Qt 4.7

Flipable {
    id: container
    anchors.top: parent.top

    property bool flipped: false
    property int xAxis: 0
    property int yAxis: 1
    property int angle: -180

    transform: Rotation {
        id: rotation; origin.x: container.width / 2; origin.y: container.height / 2
        axis.x: container.xAxis; axis.y: container.yAxis; axis.z: 0
    }

    states: State {
        name: "back"; when: container.flipped
        PropertyChanges { target: rotation; angle: container.angle }
    }

    transitions: Transition {
        ParallelAnimation {
            NumberAnimation { target: rotation; properties: "angle"; duration: 800 }
            SequentialAnimation {
                NumberAnimation { target: container; property: "scale"; to: 0.8; duration: 300 }
                NumberAnimation { target: container; property: "scale"; to: 1.0; duration: 300 }
            }
        }
    }
}
