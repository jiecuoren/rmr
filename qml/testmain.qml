import QtQuick 1.0
import MReader 1.0

Rectangle {
    width: 360
    height: 640


    property string tt: ""

    AuthenticationMgr {
        id: google

        onLoadFinished: {
            console.log("AuthenticationMgr onLoadFinished");
            console.log("aRetStr is " + aRetStr);

            var retObject = eval('(' + aRetStr + ')')

            var index = 0;

            while (index < retObject.subscriptions.length)
            {
                var mTitle = "";
                var mLink = "";
                var mCatName = "";

                mTitle = retObject.subscriptions[index].title;
                mLink = retObject.subscriptions[index].id;

                if(typeof(retObject.subscriptions[index].categories[0]) === "object")
                {
                    mCatName = retObject.subscriptions[index].categories[0].label;
                }
                google.insertMap(mCatName, mTitle, mLink);
                ++ index;
            }

            google.composeOpml();

        }
    }

    Rectangle {
        id: leftBtn
        width: 180
        height: 60
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        color: "red"

        MouseArea{
            anchors.fill: parent
            onClicked: {
                console.log("left button clicked");
                google.doLogin("liam.goolei@gmail.com", "goo841114");
//                tt = Qt.btoa("goo841114");
//                console.log("ttddddd is " + tt )
//                console.log(Qt.btoa("jiecuoren@gmail.com"));
            }
        }
    }

    Rectangle {
        id: rightBtn
        width: 180
        height: 60
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "yellow"

        MouseArea{
            anchors.fill: parent
            onClicked: {
                console.log("right button clicked");
//                google.test();
                google.getFullList();
//                console.log("tt is " + tt )
//                console.log(Qt.atob(tt));
            }
        }
    }


}
