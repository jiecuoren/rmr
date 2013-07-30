#include <QFile>
#include <QXmlStreamReader>
#include <QRegExp>
#include <QStringList>
#include <QUrl>
#include <QXmlStreamNamespaceDeclaration>
#include <QStringRef>
#include <QtAlgorithms>
#include "mrdetailmodel.h"
#include "mrdebug.h"
#include "imagemgr.h"
#include "rsschannelfile.h"
#include "commonvalues.h"
#include "mrapplicationcontroller.h"
#include "mrimagelistmodel.h"

MRDetailModel::MRDetailModel(QObject *aParent) : QAbstractListModel (aParent),
    mRssChannelFile(NULL),
    mImgProvider(NULL)
{
    QHash<int, QByteArray> roles;
    roles[EOffset] = "offset";
    roles[ETitle] = "title";
    roles[EDescription]="description";
    roles[EContent]="content";
    roles[EFullUrl]="fullUrl";
    roles[EIconUrl]="iconUrl";
    roles[EPubDate]="pubDate";
    roles[EIconUrlList] = "iconUrlList";
    setRoleNames(roles);

    mImgProvider = ImageMgr::openInstance();
    connect(mImgProvider, SIGNAL(loadingFinished(QString, int)), this, SLOT(onImgFinished(QString, int)));
    connect(mImgProvider, SIGNAL(loadingError(QString)), this, SLOT(onImgError(QString)));
}

MRDetailModel::~MRDetailModel()
{
    cleanup();
    mImgProvider->cancelAllRequest();
    mImgProvider->closeInstance();
    mImgProvider = NULL;
    mRssChannelFile->disconnect(this);
}

int MRDetailModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent)
    return mItemList.count();
}

DetailItem* MRDetailModel::data(const int row)const
{
    LOG_METHOD;

    if (mItemList.isEmpty() || row >= mItemList.count() || row < 0)
        return NULL;

    DetailItem *dobj = mItemList.at(row);

    if (!dobj->isInited())
    {
        mRssChannelFile->constructItem(*dobj, dobj->offset());
    }

    return dobj;
}


QVariant MRDetailModel::data(const QModelIndex &aIndex, int aRole) const
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
    DetailItem* dobj = mItemList.at(row);

    switch (aRole)
    {
    case Qt::DisplayRole:
    case EOffset:
        rv = dobj->mOffset;
        break;
    case ETitle:
        rv = dobj->mTitle;
        break;
    case EIconUrl:
        {
            // if no icon url, we don't need default icon
            if( !dobj->mIconUrl.isEmpty())
            {
                QString localpath = mImgProvider->getLocalPath(dobj->iconPath(), EIcon);
                if (localpath.isEmpty())
                {
                    //need to start download icons
                    ImageRequestToken* token = mImgProvider->addImageRequest( dobj->iconPath(), EIcon );
                    connect( token, SIGNAL(finished(QString)), this, SLOT(updateItemImage(QString)) );
                    localpath = KDefaultIcon;
                }
                dobj->setIconLocalPath( localpath );

                rv = dobj->mIconLocal;
            }
            else
            {
                rv = QString();
            }
        }
        break;
    default:
        break;
    }

    return rv;
}

void MRDetailModel::onImgFinished(QString aStr, int aIndex)
{
    LOG_METHOD;

    if(aStr.isEmpty())
    {
        return;
    }

    mItemList[aIndex]->mIconUrl = aStr;
    QModelIndex topLeft = createIndex(aIndex,0);
    emit dataChanged(topLeft, topLeft);
}

void MRDetailModel::onImgError(QString aStr)
{
    Q_UNUSED(aStr);
}


void MRDetailModel::refresh()
{
    LOG_METHOD;

    reset();
}

void MRDetailModel::imageDownloaded(QString aImgPath, int aIndex)
{
    if(aImgPath.isEmpty())
    {
        return;
    }

    mItemList[aIndex]->mIconLocal = aImgPath;
    QModelIndex topLeft = createIndex(aIndex,0);
    emit dataChanged(topLeft, topLeft);
}

void MRDetailModel::setFeedSource(QString url, QString filepath)
{
    LOG_METHOD;

    mRssFilepath = filepath;

    if (mRssChannelFile != NULL) {
        mRssChannelFile->closeFileHandler();
        mRssChannelFile->disconnect();
    }
    mRssChannelFile = MRApplicationController::getController()->channelFile(url, filepath);

    cleanup();

    mRssChannelFile->getFullItemlist(mItemList);

    mRssChannelFile->openFileHandler();

    refresh();
}

void MRDetailModel::updateItemImage(QString aStr)
{
    Q_UNUSED(aStr);

    foreach(DetailItem* item, mItemList) {
        if (item->iconLocalPath().isEmpty()) {
            QString localpath = mImgProvider->getLocalPath(item->iconPath(), EIcon);
            if (!localpath.isEmpty())
            {
               item->setIconLocalPath( localpath );

               QModelIndex tempIndex = createIndex(item->mIndex,0);
               emit dataChanged(tempIndex, tempIndex);
               break;
            }
        }
    }
}

void MRDetailModel::cleanup()
{
    qDeleteAll(mItemList.begin(), mItemList.end());
    mItemList.clear();
}

DetailItem::DetailItem(QObject* parent )
    :QObject(parent),mInited(false)
{
    mImageListModel = new MrImageListModel(this);
}

DetailItem::~DetailItem()
{
    if (mImageListModel != NULL)
        delete mImageListModel;
    mImageListModel = NULL;
}

void DetailItem::appendImageList(const QString& imageurl)
{
    mImageListModel->appendItem(imageurl);
}

void DetailItem::clearImageList()
{
    mImageListModel->clearItem();
}

void DetailItem::setDescription(const QString &description)
{
    mDescription = description;
    mInited = true;
}

void DetailItem::setIconPath(const QString& path)
{
    mIconUrl = path;
}
