#ifndef RSSCHANNELFILE_H
#define RSSCHANNELFILE_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWidget>
#include <QBuffer>
#include <QXmlStreamReader>
#include <QUrl>
#include <QFile>
#include <QListIterator>

class DataProvider;
class RSSFileDownloader;
class RSSRequest;

// TODOs:change it to structure
class RSSItemShort
{
public:
    RSSItemShort(){}

    QString getTitle() {return mTitle;}
    QString getIconPath() {return mIconPath;}
    QString getPubDate() {return mPubDate;}

    void setTitle(const QString title) {mTitle = title;}
    void setIconPath(const QString path) {mIconPath = path;}
    void setPubDate(const QString date) {mPubDate = date;}

public:
    QString mTitle;
    QString mIconPath;
    QString mPubDate;
    QString mGuid;
    QString mExcerpt;
    int     mOffset;
};

/*
class RSSItemFull : public QObject
{
    Q_OBJECT
// to be implemented
};
*/

class DetailItem;

class RSSChannelFile : public QObject
{
    Q_OBJECT

public:
    RSSChannelFile(QObject *parent = 0);
    ~RSSChannelFile();

    //for testing local file only
    void setSource(QString filepath);

    void setSourceUrl(QString url, QString filepath = "");
    QString sourceUrl()const{return mUrl;}

    void setParam(QString urlString, QString filePath);    

    bool getFullItemlist(QList<DetailItem*>& itemList);

    QListIterator<RSSItemShort>* getItemListIterator();
    void cancelAllRequest();

    bool openFileHandler();
    void closeFileHandler();

    bool constructItem(DetailItem& detailItem, int byteStart);

public slots:
    void update();  //to be obsolete
    void finished(QByteArray aData);

signals:
    void loadingStartted();
    void loadingError();
    void loadingFinished();
    void loadingCancel();
    void newItemsArrived(int num);
    void error(QString aErrorStr);

private:
    int  merge(const QByteArray& data);
    void get(QString url);
    bool preload();
    void preloadItem();
//    DetailItem* getFullItem(bool fullLoad);
    void removeEmbedContent(QString& content);
    void cleanup();
    void processXmlNewItem(QXmlStreamReader& reader, QXmlStreamWriter& writer, RSSItemShort& item);
    void processXmlOldItem(QXmlStreamReader& reader, QXmlStreamWriter& writer, RSSItemShort& item);
    void processHostName(QXmlStreamReader& reader, QXmlStreamWriter& writer);
    QString getHostName(QString url);
    int processXmlFeeds(QXmlStreamReader& reader, QXmlStreamWriter& writer, QFile& bakfile);
    QStringList extractImagePaths(QString text); //extract image path from xml content
    bool isValidUrl(QString url);
    inline bool userDefined(QString url);
    bool doConstructItem(QXmlStreamReader& reader, DetailItem& detailItem);

private:
    QString     mFilepath;
    QString     mUrl;
    QString     mHostName;

    QFile   mFileHandle;

    QByteArray httpData;

    QList<RSSItemShort> mItemList;
    QList<RSSItemShort> mItemNewList;

    bool                mLoaded;
    QXmlStreamReader    mXmlReader;
//    DataProvider* mDataProvider;
    RSSRequest* mRSSRequest;
    RSSFileDownloader* mRSSFileDownloader;
};


#endif // RSSCHANNELFILE_H
