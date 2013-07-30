import QtQuick 1.0

Item {
    width: parent.width
    height: defaultHeight

    property bool showing: true
    property variant defaultHeight: appState.titleBarHeight
    property string titleText: appState.appName
    property int fontSize: appState.titleFontSize


    property int preMinutes: -1

    function getTime()
    {
        var date = new Date;

        var hours = date.getHours()
        var mins = date.getMinutes()

        preMinutes = mins

        hours = (hours < 10) ? ('0' + hours) : hours
        mins = (mins<10) ? ('0' + mins) : mins

        return hours + ':' + mins

    }

    onShowingChanged: {
        if (showing){
            height = defaultHeight
        }else{
            height = 0
        }
    }

    Rectangle{
        id: bk
        anchors.fill: parent;
        gradient: Gradient {
            GradientStop { position: 0.0; color: "black" }
            GradientStop { position: 0.5; color: "#333333" }
            GradientStop { position: 0.9; color: "#666666" }
            GradientStop { position: 1.0; color: "#888888" }
        }
    }

    Text {
        id: title
        text: titleText
        anchors{
            margins: 5
            bottom: parent.bottom
        }
        color: "white"
        font{pixelSize: fontSize}
    }

    Text {
        id: time
        text: getTime()
        color: "white"
        anchors{
            margins: 5
            right: parent.right
            bottom: parent.bottom
        }
        font{pixelSize: fontSize}
    }

    Timer{
        id: timer
        interval: 2000 // one second
        running: true
        repeat: true
        onTriggered: {
            var date = new Date
            if (preMinutes != date.getMinutes() )
                time.text = getTime()
        }
    }

    Behavior on height {
        // Animate subitem expansion. After the final height is reached,
        // ensure that it is visible to the user.
        ParallelAnimation {
            NumberAnimation { easing.type: Easing.InOutQuad }
            ScriptAction { script: ;}
        }
    }
}
