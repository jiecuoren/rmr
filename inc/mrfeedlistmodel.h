#ifndef MRFEEDLISTMODEL_H
#define MRFEEDLISTMODEL_H

#include <QAbstractItemModel>


#include "feedobject.h"



class FeedObject;
class MRFeedListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString foldername READ folderName)
    Q_PROPERTY(int count READ count)
    Q_ENUMS(Roles)

public:
    enum Roles { ETitle       = Qt::UserRole+1,
                 EType        = Qt::UserRole+2,
                 EUrl         = Qt::UserRole+3,
                 EIconUrl     = Qt::UserRole+4,
                 EHtmlUrl     = Qt::UserRole+5,
                 EDescription = Qt::UserRole+6,
                 ESubscribed  = Qt::UserRole+7,
                 ELocalPath   = Qt::UserRole+8
               };

public:
    Q_DISABLE_COPY(MRFeedListModel)

    explicit MRFeedListModel(const QString& foldername = QString(), QObject *parent = NULL);
    ~MRFeedListModel();

    QString folderName()const {return mFolderName;}

    int rowCount(const QModelIndex &parent) const;

    Q_INVOKABLE QString data(const int row, int role) const;
    Q_INVOKABLE FeedObject* data(const int row)const;

    QVariant data(const QModelIndex &index, int role) const;
    int count() const { return rowCount(QModelIndex()); }

    // model function
    void appendItem(FeedObject *item);
    void removeItem(const int aIndex);
    void refresh();
    void clearItems();
private:


    const QString mFolderName;
    QList<FeedObject*> m_items;
};

#endif // MRFEEDLISTMODEL_H
