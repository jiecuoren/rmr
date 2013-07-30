import QtQuick 1.0

Rectangle {
    id: container
    width: parent.width; height: 0
    color: "black"
    opacity: 0.7


    property bool showing: false
    property variant defaultHeight: appState.toolBarHeight
    property int buttonWidth: defaultHeight
    property int buttonTopMargin: (defaultHeight - buttonWidth) >> 1
    property bool showingBackBtn: false
    property int margin: 8
    property string settingBtnSource: "../resources/icon/setting.png"
    property string settingBtnPressedSource: "../resources/icon/setting-press.png"

    property string backBtnSource: "../resources/icon/back.png"
    property string backBtnPressedSource: "../resources/icon/back-press.png"

    property string exitBtnSource: "../resources/icon/exit.png"
    property string exitBtnPressedSource: "../resources/icon/exit-press.png"

    property string infoBtnSource: "../resources/icon/info.png"
    property string infoBtnPressedSource: "../resources/icon/info-press.png"

    signal settingClicked
    signal midBtnClicked

    onShowingChanged: {
        if (showing){
            height = defaultHeight
        }else{
            height = 0
        }
    }

    function updateShowingBtn(bBackBtn){
        showingBackBtn = bBackBtn
    }

    Button{
        id: settingBtn
        imgWidth: buttonWidth
        image: showingBackBtn ? backBtnSource : settingBtnSource
        image_press: showingBackBtn? backBtnPressedSource : settingBtnPressedSource
        anchors{
            leftMargin: 19
            topMargin: buttonTopMargin
            top: parent.top
            left: parent.left
        }

        onClicked: {
            container.settingClicked()
        }
    }

    Button{
        id: exit
        imgWidth: buttonWidth
        image: exitBtnSource
        image_press: exitBtnPressedSource
        anchors{
            rightMargin: 19
            topMargin: buttonTopMargin
            top: parent.top
            right: parent.right
        }

        onClicked: {
            Qt.quit()
        }
    }

    Button{
        id: info
        imgWidth: buttonWidth
        image: infoBtnSource
        image_press: infoBtnPressedSource
        anchors{
            topMargin: buttonTopMargin
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }

        onClicked: {
            midBtnClicked();
        }
    }

    MouseArea{id:bk; anchors.fill: parent; z: -1}


    Behavior on height {
        // Animate subitem expansion. After the final height is reached,
        // ensure that it is visible to the user.
        ParallelAnimation {
            NumberAnimation { easing.type: Easing.InOutQuad }
            ScriptAction { script: ;}
        }
    }


}
