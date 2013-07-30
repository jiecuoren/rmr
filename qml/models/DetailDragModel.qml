import QtQuick 1.0

Item {   

    property alias model: dragModel

    ListModel {
        id: dragModel

        Component.onCompleted: {
            dragModel.append({"detailModel": undefined});
            dragModel.append({"detailModel": undefined});
            dragModel.append({"detailModel": undefined});
        }
    }

    property string test1: "text1"
    property string test2: "text2"
    property string test3: "text3"

    DetailComponent {
        id: component1
        textStr: "Component1"
    }

    DetailComponent {
        id: component2
        textStr: "Component2"
    }

    DetailComponent {
        id: component3
        textStr: "Component3"
    }


}
