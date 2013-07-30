import QtQuick 1.0

XmlListModel {
    id: model

    property string feedUrl: ""
    property bool loading: status == XmlListModel.Loading

    source: feedUrl
    query: "/rss/channel/item"
    XmlRole { name: "title"; query: "title/string()" }
    XmlRole { name: "description"; query: "description/string()" }
    XmlRole { name: "url"; query: "link/string()" }
    XmlRole { name: "enclosureUrl"; query: "enclosure/@url/string()" }
    XmlRole { name: "enclosureType"; query: "enclosure/@type/string()" }
    XmlRole { name: "pubDate"; query: "pubDate/string()" }

    namespaceDeclarations: "declare namespace content='http://purl.org/rss/1.0/modules/content/';"

    //add img
    XmlRole { name: "imgBack"; query: "content:encoded/string()" }

    onStatusChanged: {
        if (status == XmlListModel.Ready) {
            //console.log("FeedModel Status: ready")
        } else if (status == XmlListModel.Error) {
            //console.log("FeedModel Status: error", errorString())
        } else if (status == XmlListModel.Loading) {
            //console.log("FeedModel Status: loading")
        }
    }

    onFeedUrlChanged: {
        //console.log("Feed url changed: "+feedUrl)
    }


}
