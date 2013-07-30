#ifndef MRXMLDATAMODEL_H
#define MRXMLDATAMODEL_H

// INCLUDES
#include <QDeclarativeItem>
#include <QDomDocument>

class FeedObject;
class MRFeedListModel;
class MRApplicationController;

class MrOpmlConfig: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString modelName READ modelName)

public:
    explicit MrOpmlConfig(QObject* parent = 0);
    ~MrOpmlConfig();

    Q_INVOKABLE MRFeedListModel* getFolderListModel();
    Q_INVOKABLE MRFeedListModel* getFeedListByFolderName(const QString& folder);

    // called when click add more button
    Q_INVOKABLE bool addItemByUser(const QString& aFeedUrl, const QString& aFeedName,
                             const QString& aPageName);

    Q_INVOKABLE bool deleteItemByUser(const QString& aItemName, const QString& aPageName, const int aItemIndex);

    Q_INVOKABLE bool renameItemByUser(const QString& aOldName, const QString& aNewName,
                             const QString& aPageName, const int aItemIndex);

    Q_INVOKABLE bool addItemBySync(const QString& aFeedUrl, const QString& aFeedName,
                                   const QString& aPageName);

    QString modelName()const{ return mModelName; }

    Q_INVOKABLE void initUserFolder(QString aUserName);
    Q_INVOKABLE void loadOpml();
    static MrOpmlConfig* openInstance();
    void closeInstance();
    QString getOpmlPath();
signals:
    void loaded();
    void error(const QString errorstr);

private:    
    bool mrLoadXml();

    void mrSaveXml();

    bool initFolderList(const QDomElement& domRoot);

    void addFeedItem(MRFeedListModel* pNod, QDomElement& element);

    MRFeedListModel* feedList(const QString& folder) const;   

    QDomElement getFolderElement(const QString& folder);

    void addConfigAttribute(QDomElement& domRoot);

    bool doSubscribed(QDomElement& domElement, bool subscribed);

    void parseFolderElement(const QDomElement& element);

    void parseModelName(const QDomElement& domRoot);

    void clearFeedList();

    //call this function to sync Google account
    void syncGoogle();

    // control
    void validLocalPath(FeedObject *item, QDomElement& element);

    uint getNextSerialNum();
    void updateConfigPart(bool fromOpml);
    void adjustLocalPath();

    // private variable
    QDomDocument mDomDocument;

    uint mSerialNum;
    bool mModified;

    QString mModelName;

    MRFeedListModel* mFolderListModel; // defined for folder list model

    QList<MRFeedListModel*> mFeedModelList;

    static MrOpmlConfig* mThis;

    QString mLocalDir;
    QString mUserName;
    QString mAppFilePath;
    QString cfgfile;
    QString cfgfileout;
};

#endif // MRXMLDATAMODEL_H
