#ifndef MRIMAGELISTMODEL_H
#define MRIMAGELISTMODEL_H

#include <QAbstractItemModel>

class ImageMgr;
class MrImageListModel;


class MrImageItem : public QObject
{
    Q_OBJECT

public:
    explicit MrImageItem(const int index,
                         const QString url,
                         const QString local = QString()
                         ):mIndex(index), mUrl(url), mLocal(local){}    

    void setLocalPath(const QString localpath) {mLocal = localpath;}

    void setParentModel(MrImageListModel* parent) {mParentModel = parent;}

    QString url()const {return mUrl;}
    QString localpath()const {return mLocal;}

private slots:
    void imageDownloaded(QString local);

private:
    int mIndex;
    MrImageListModel* mParentModel;

    QString mUrl;
    QString mLocal;
};

class MrImageListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    Q_DISABLE_COPY(MrImageListModel)

    explicit MrImageListModel(QObject *parent = 0);
    ~MrImageListModel();

    Q_INVOKABLE void downloadImage(int index);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &aIndex, int aRole) const;
    void appendItem(const QString& imageurl);
    void clearItem(){mItemList.clear();}

public:
    enum Roles { EImageUrl    = Qt::UserRole+1,
                 ELocalPath   = Qt::UserRole+2
               };

private:
    void imageDownloaded(QString aImgPath, int aIndex);
    void onImgError(QString aStr);

    QList<MrImageItem*> mItemList;

    ImageMgr* mImgMgr;

    friend class MrImageItem;
};

#endif // MRIMAGELISTMODEL_H
