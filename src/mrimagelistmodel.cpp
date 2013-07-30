#include <QHash>

#include "mrdebug.h"
#include "imagemgr.h"
#include "commonvalues.h"
#include "mrimagelistmodel.h"

MrImageListModel::MrImageListModel(QObject *parent) :
    QAbstractListModel(parent), mImgMgr(NULL)
{
    QHash<int, QByteArray> roles;
    roles[EImageUrl] = "imageurl";
    roles[ELocalPath] = "localpath";
    setRoleNames(roles);

}

MrImageListModel::~MrImageListModel()
{
    if (mImgMgr != NULL)
    {
        mImgMgr->cancelAllRequest();
        mImgMgr->closeInstance();
        mImgMgr = NULL;
    }

    qDeleteAll(mItemList.begin(), mItemList.end());
    mItemList.clear();
}

int MrImageListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return mItemList.count();
}

QVariant MrImageListModel::data(const QModelIndex &aIndex, int aRole) const
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

    MrImageItem* obj = mItemList.at(row);

    switch (aRole)
    {
    case Qt::DisplayRole:
    case EImageUrl:
        rv = obj->url();
        break;
    case ELocalPath:
        rv = obj->localpath();
        break;
    default:
        rv = QString();
        break;
    }
    return rv;
}

void MrImageListModel::downloadImage(int index)
{

    MrImageItem* item = mItemList.at(index);

    if (!item->localpath().isEmpty())
        return;

    QString local = mImgMgr->getLocalPath(item->url(), E150150);

    LOG_FORMAT("MrImageListModel, return local: ", local.toLatin1().data() );

    if (local.isEmpty())
    {
        item->setParentModel(this);
        ImageRequestToken* token = mImgMgr->addImageRequest( item->url(), E150150 );
        connect( token, SIGNAL(finished(QString)), item, SLOT(imageDownloaded(QString)) );
    }
    item->setLocalPath(local);
}

void MrImageListModel::appendItem(const QString& imageurl)
{
    if (NULL == mImgMgr)
    {
        mImgMgr = ImageMgr::openInstance();
    }

    MrImageItem* item = new MrImageItem(mItemList.count(), imageurl);
    item->setLocalPath(mImgMgr->getLocalPath(imageurl, E150150));
    mItemList.append(item);
}

void MrImageListModel::imageDownloaded(QString aImgPath, int aIndex)
{
    if(aImgPath.isEmpty())
    {
        return;
    }

    QModelIndex index = createIndex(aIndex,0);
    emit dataChanged(index, index);
}

void MrImageListModel::onImgError(QString aStr)
{
    Q_UNUSED(aStr);
}

void MrImageItem::imageDownloaded(QString local)
{
    mLocal = local;
    mParentModel->imageDownloaded(local, mIndex);
}
