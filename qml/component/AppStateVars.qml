import QtQuick 1.0

QtObject {
    property string appName: "MReader"
    property string version: "1.0.1"

    property bool isReading: false

    property bool loading: false
    property bool showBackButton: false;
    property bool fromLeft: false
    property string currentViewName: ""
    property string cameFromView: ""

    // view IDs
    property int kSplashView: 0
    property int kMainView: 1
    property int kAboutView: 2
    property int kLoginView: 3

    // Contains the string currently shown
    property string currentTitle: "MReader"
    property string themeDir: ""

    // Define the list style
    property string bgImageSubItem: '../resources/icon/list_item.png'
    property string arrow: '../resources/icon/arrow.png'
    property string refresh: '../resources/icon/loading.gif'

    property variant titleBarHeight: 30.0
    property variant titleFontSize: 20.0

    property variant toolBarHeight: 50.0

    property variant cursorBarHeight: 45.0
    property variant cursorItemWidth: 87.0

    property variant detailPicsHeight: 150.0

    property string headerItemFontName: "Helvetica"
    property int headerItemFontSize: 16
    property color headerItemFontColor: "white"
    property variant headerItemHeight: 24.0

    property string subItemFontName: "Helvetica"
    property int subItemFontSize: 15
    property color subItemFontColor: "white"
    property variant subItemHeight: 120.0
    property variant subItemTextSpace: 6.0
    property variant subItemSpace: 0
}

