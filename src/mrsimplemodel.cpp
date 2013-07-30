#include <QFile>
#include <QXmlStreamReader>
#include <QRegExp>
#include <QStringList>
#include <QUrl>
#include <QXmlStreamNamespaceDeclaration>
#include <QStringRef>
#include <QtAlgorithms>
#include <QThread>
#include "mrsimplemodel.h"
#include "mrdebug.h"
#include "imagemgr.h"
#include "rsschannelfile.h"
#include "commonvalues.h"
#include "mrapplicationcontroller.h"

MRSimpleModel::MRSimpleModel(QObject *aParent) : QAbstractListModel (aParent),
            mImgMgr(NULL), mRssChannelFile(NULL)
{
    QHash<int, QByteArray> roles;
    roles[ETitle] = "title";
    roles[EIconUrl]="iconUrl";
    roles[EExcerpt] = "excerpt";
    roles[EPubDate] = "pubDate";
    setRoleNames(roles);

    mImgMgr = ImageMgr::openInstance();
    connect(mImgMgr, SIGNAL(loadingFinished(QString, int)), this, SLOT(onImgFinished(QString, int)));
    connect(mImgMgr, SIGNAL(loadingError(QString)), this, SLOT(onImgError(QString)));
    connect(mImgMgr, SIGNAL(cancel()), this, SLOT(onImgReqCancel()));

}

MRSimpleModel::~MRSimpleModel()
{
    cleanup();
    mImgMgr->cancelAllRequest();
    mImgMgr->closeInstance();
    mImgMgr = NULL;
    mRssChannelFile->disconnect(this);
}

int MRSimpleModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent)
    return mItemList.count();
}

QVariant MRSimpleModel::data(const QModelIndex &aIndex, int aRole) const
{
    if (!aIndex.isValid())
    {
        return QVariant();
    }

    if (aIndex.row() >= mItemList.count())
    {
        return QVariant();
    }

    if(mItemList.isEmpty())
    {
        return QVariant();
    }

    QVariant rv;
    int row = aIndex.row();

    switch (aRole)
    {
    case ETitle:
        rv = mItemList.at(row)->mTitle;
        break;

    case EIconUrl:
        if(!mItemList.at(row)->mIconUrl.isEmpty())
        {
            mItemList.at(row)->mIconLocal = mImgMgr->getLocalPath( mItemList.at(row)->mIconUrl, EIcon);
            if(mItemList.at(row)->mIconLocal.isEmpty())
            {
                mItemList.at(row)->mIconLocal = KDefaultIcon;

                if(!mInReqList.contains(row))
                {
                    ImageRequestToken* token = mImgMgr->addImageRequest( mItemList.at(row)->mIconUrl, EIcon );
                    connect( token, SIGNAL(finished(QString)), mItemList.at(row), SLOT(imageDownloaded(QString)) );
                    mInReqList.append(row);
                }

            }
        }
        else
        {
            mItemList.at(row)->mIconLocal = "";
        }

        rv = mItemList.at(row)->mIconLocal;

        break;

    case EExcerpt:
        rv = mItemList.at(row)->mExcerpt;
        break;

    case EPubDate:
        rv = mItemList.at(row)->mPubDate;
        break;

    default:
        break;
    }

    return rv;
}

void MRSimpleModel::onImgFinished(QString aStr, int aIndex)
{
    LOG_METHOD;

    if(aStr.isEmpty())
    {
        return;
    }

    mItemList[aIndex]->mIconUrl = aStr;
    QModelIndex tempIndex = createIndex(aIndex,0);
    emit dataChanged(tempIndex, tempIndex);
}

void MRSimpleModel::onImgError(QString aStr)
{
    Q_UNUSED(aStr);
}


void MRSimpleModel::refresh()
{
    LOG_METHOD;

    mRssChannelFile->update();

}

void SimpleItem::imageDownloaded(QString aImgPath)
{
    LOG_METHOD;
    mModel->imageDownloaded(aImgPath, mIndex);
}

void MRSimpleModel::imageDownloaded(QString aImgPath, int aIndex)
{
    LOG_METHOD;

    if(aImgPath.isEmpty())
    {
        return;
    }

    mItemList[aIndex]->mIconLocal = aImgPath;
    QModelIndex topLeft = createIndex(aIndex,0);
    emit dataChanged(topLeft, topLeft);
    mInReqList.removeOne(aIndex);
}

void MRSimpleModel::insertNewItems( int number )
{
    LOG_METHOD;
    qDebug()<<"MRFeedItemsListModel::insertNewItems items:"<<number<<"\n";

    if (number == 0)
    {
        // TODOs: do we need to notify the fetching progressive icon to stop?
        // Yes, we need.
        emit rssLoadingStopped();
        return;
    }

    QListIterator<RSSItemShort>* itor;
    itor = mRssChannelFile->getItemListIterator();

    mItemList.clear();

    //control the number of items here.
    while (itor->hasNext())
    {
        RSSItemShort temp = itor->next();
        SimpleItem* item = new SimpleItem(this);

        item->mTitle = temp.getTitle();
        item->mIconUrl = temp.getIconPath();
        item->mPubDate = temp.getPubDate();
        item->mExcerpt = temp.mExcerpt;

        item->mIndex = mItemList.count();
        mItemList.append(item);

    }

    delete itor;
    itor = NULL;
    // reset the model
    emit rssLoadingStopped();
    reset();
}

void MRSimpleModel::setFeedSource(QString url, QString filepath)
{
    LOG_METHOD;

    mRssFilepath = filepath;

    mRssChannelFile = MRApplicationController::getController()->channelFile(url, filepath);
    connect( mRssChannelFile, SIGNAL(newItemsArrived(int)), this, SLOT(insertNewItems(int)) );
    connect( mRssChannelFile, SIGNAL(loadingStartted()), this, SIGNAL(rssLoadingStartted()) );
    connect( mRssChannelFile, SIGNAL(error(QString)), this, SIGNAL(rssLoadingError(QString)) );
    connect( mRssChannelFile, SIGNAL(loadingCancel()), this, SIGNAL(rssLoadingCancel()) );

    QListIterator<RSSItemShort>* itor;
    itor = mRssChannelFile->getItemListIterator();

    qDebug() << "MRSimpleModel::setFeedSource..." ;

    cleanup();

    if(NULL == itor)
    {
        qDebug() << "MRSimpleModel::setFeedSource no local file, need refresh" ;
        reset();
        refresh();
        return;
    }  

    //control the number of items here.
    while (itor->hasNext())
    {
        RSSItemShort temp = itor->next();
        SimpleItem* item = new SimpleItem(this);

        item->mTitle = temp.getTitle();
        item->mIconUrl = temp.getIconPath();
        item->mPubDate = temp.getPubDate();
        item->mExcerpt = temp.mExcerpt;

        item->mIndex = mItemList.count();
        mItemList.append(item);

    }

    delete itor;
    itor = NULL;
    reset();
}

void MRSimpleModel::cleanup()
{
    LOG_METHOD;

    qDeleteAll(mItemList.begin(), mItemList.end());
    mItemList.clear();
    mInReqList.clear();
}

void MRSimpleModel::onImgReqCancel()
{
    LOG_METHOD;
    mInReqList.clear();
}

SimpleItem::SimpleItem(MRSimpleModel *model) :
    mTitle(""),
    mIconUrl(""),
    mIconLocal(""),
    mExcerpt(""),
    mPubDate(""),
    mIndex(0)
{
    if(NULL != model)
    {
        mModel = model;
    }
}

