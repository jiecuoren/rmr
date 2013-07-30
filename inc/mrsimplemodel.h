#ifndef _MR_SIMPLE_MODEL
#define _MR_SIMPLE_MODEL

#include <QAbstractItemModel>
#include <QStringList>
//#include "rsschannelfile.h"

class ImageMgr;
class RSSChannelFile;
class QXmlStreamReader;
class MRSimpleModel;
class QThread;

class SimpleItem : public QObject
{
    Q_OBJECT

public:
    SimpleItem(MRSimpleModel* model);

    inline void clear()
    {
        mTitle.clear();
        mIconUrl.clear();
        mIconLocal.clear();
        mExcerpt.clear();
        mIndex = 0;
        mPubDate.clear();
    }

private slots:
    void imageDownloaded(QString aImgPath);

protected:
    QString mTitle;
    QString mIconUrl;
    QString mIconLocal;
    QString mExcerpt;
    QString mPubDate;

    //Is this useful?
    int mIndex;
    MRSimpleModel* mModel;

friend class     MRSimpleModel;
};

class MRSimpleModel : public QAbstractListModel
{
    Q_OBJECT

public:
    MRSimpleModel(QObject *aParent = NULL);
    ~MRSimpleModel();

    // Form Base class
    int rowCount(const QModelIndex &aParent = QModelIndex()) const;
    QVariant data(const QModelIndex &aIndex, int aRole = Qt::DisplayRole) const;
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setFeedSource(QString url, QString filepath);
signals:
    void rssLoadingStartted();
    void rssLoadingStopped();
    void rssLoadingError(QString aErrorStr);
    void rssLoadingCancel();
private:
    void cleanup();

private slots:

    void onImgFinished(QString aStr, int aIndex);
    void onImgError(QString aStr);
    void insertNewItems( int number );
    void onImgReqCancel();

public:
    void imageDownloaded(QString aImgPath, int aIndex);

private:
    enum Roles { ETitle = Qt::UserRole+1,
                 EIconUrl = Qt::UserRole+2,
                 EExcerpt = Qt::UserRole+3,
                 EPubDate = Qt::UserRole+4
               };

    QList<SimpleItem*> mItemList;
    ImageMgr* mImgMgr;
    RSSChannelFile* mRssChannelFile;
    QString mRssFilepath;
    mutable QList <int> mInReqList;
};

#endif /* _MR_SIMPLE_MODEL */


