#ifndef MRDETAILMODEL_H
#define MRDETAILMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include <QDeclarativeListProperty>

class ImageMgr;
class MrImageListModel;
class RSSChannelFile;
class QXmlStreamReader;
class MRDetailModel;

class DetailItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int offset READ offset CONSTANT)
    Q_PROPERTY(QString title READ titleStr CONSTANT)
    Q_PROPERTY(QString link READ linkStr CONSTANT)
    Q_PROPERTY(QString pubDate READ pubDateStr CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT )
    Q_PROPERTY(MrImageListModel* imagelist READ imageList CONSTANT)

public:
    DetailItem(QObject* parent = 0);
    ~DetailItem();

    bool isInited()const {return mInited;}

    int     offset()const {return mOffset;}
    QString titleStr()const {return mTitle;}
    QString iconPath()const {return mIconUrl;}
    QString iconLocalPath()const {return mIconUrl;}
    QString linkStr()const {return mLink;}
    QString description()const {return mDescription;}
    QString pubDateStr()const {return mPubDate;}
    MrImageListModel* imageList()const {return mImageListModel;}

    void setIndex(const int index) {mIndex = index;}
    void setOffset(const int offset) {mOffset = offset;}
    void setTitle(const QString& title) {mTitle = title;}
    void setLink(const QString& link) {mLink = link;}    
    void setIconLocalPath(const QString& localpath) {mIconLocal = localpath;}
    void setPubDate(const QString& pubdate) {mPubDate = pubdate;}
    void setIconPath(const QString& path);
    void setDescription(const QString& description);
    void appendImageList(const QString& imageurl);
    void clearImageList();

protected:
    int     mOffset;
    QString mTitle;
    QString mLink;
    QString mIconUrl;
    QString mIconLocal;
    QString mPubDate;
    QString mDescription;

    MrImageListModel* mImageListModel;

    bool mInited;
    int mIndex;

friend class     MRDetailModel;
};

class MRDetailModel : public QAbstractListModel
{
    Q_OBJECT

public:
    MRDetailModel(QObject *aParent = NULL);
    ~MRDetailModel();

    //... Form Base class
    int rowCount(const QModelIndex &aParent = QModelIndex()) const;
    QVariant data(const QModelIndex &aIndex, int aRole = Qt::DisplayRole) const;

    Q_INVOKABLE void cleanup();

    //... Invoked by view
    Q_INVOKABLE void setFeedSource(QString url, QString filepath);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE DetailItem* data(const int row)const;

    void imageDownloaded(QString aImgPath, int aIndex);

    //... Invoked by DetailItem
    bool itemDataUpdated(int index );

signals:
    void rssloadingStopped();

public slots:

    void updateItemImage(QString aStr);
    void onImgFinished(QString aStr, int aIndex);
    void onImgError(QString aStr);

private:
    enum Roles { EOffset      = Qt::UserRole+1,
                 ETitle       = Qt::UserRole+2,
                 EDescription = Qt::UserRole+3,
                 EContent     = Qt::UserRole+4,
                 EFullUrl     = Qt::UserRole+5,
                 EIconUrl     = Qt::UserRole+6,
                 EPubDate     = Qt::UserRole+7,
                 EIconUrlList = Qt::UserRole+8
               };

    QList<DetailItem*> mItemList;

    RSSChannelFile* mRssChannelFile;
    QString mRssFilepath;

    ImageMgr* mImgProvider;
};

#endif // MRDETAILMODEL_H


