#include <QHash>

#include "mrdebug.h"
#include "mrfeedlistmodel.h"


MRFeedListModel::MRFeedListModel(const QString& foldername, QObject *parent) :
     QAbstractListModel(parent), mFolderName(foldername)
{
    LOG_METHOD;

    LOG_FORMAT("MRFeedListModel constuct folder name:%s", mFolderName.toLatin1().data());

    QHash<int, QByteArray> roles;
    roles[ETitle] = "title";
    roles[EType] = "type";
    roles[EUrl] = "url";
    roles[EIconUrl] = "iconUrl";
    roles[EHtmlUrl] = "htmlUrl";
    roles[EDescription] = "description";
    roles[ESubscribed] = "subscribed";
    roles[ELocalPath] = "localpath";
    setRoleNames(roles);
  }

 MRFeedListModel::~MRFeedListModel()
 {
     LOG_METHOD;
     clearItems();

 }

void MRFeedListModel::clearItems()
{

    qDeleteAll(m_items.begin(), m_items.end());

    m_items.clear();
}

void MRFeedListModel::appendItem(FeedObject *item)
{
    LOG_METHOD;
    m_items.append(item);
}

void MRFeedListModel::refresh()
{
    beginResetModel();
    endResetModel();
}

int MRFeedListModel::rowCount(const QModelIndex &) const
{
    return m_items.count();
}

FeedObject* MRFeedListModel::data(const int row) const
{
    LOG_METHOD;
    FeedObject *dobj = m_items.at(row);

    LOG_FORMAT("%s",dobj->type.toLatin1().data());

    return dobj;
}

QString MRFeedListModel::data(const int row, int role) const
{
    FeedObject *dobj = m_items.at(row);

    QString result = QString();

    switch (role) {
    case Qt::DisplayRole: // The default display role now displays the first name as well
    case ETitle:
        result = dobj->title;
        break;
    case EType:
        result = dobj->type;
        break;
    case EUrl:
        result = dobj->url;
        break;
    case EIconUrl:
        result = dobj->iconUrl;
        break;
    case EHtmlUrl:
        result = dobj->htmlUrl;
        break;
    case EDescription:
        result = dobj->description;
        break;
    case ESubscribed:
        result = dobj->subscribed;
        break;
    case ELocalPath:
        result = dobj->urlLocalPath;
        break;
    default:
        result = QString();
    }

    LOG_FORMAT("FeedListModel data:%s, row:%d, role:%d", result.toLatin1().data(), row, role);
    return result;
}

QVariant MRFeedListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant(); // Return Null variant if index is invalid
    if (index.row() > (m_items.count()-1))
        return QVariant();

    return QVariant::fromValue(data(index.row(), role));
}

void MRFeedListModel::removeItem(const int aIndex)
{
    if(aIndex >= m_items.count())
    {
        return;
    }
    m_items.removeAt(aIndex);
}
