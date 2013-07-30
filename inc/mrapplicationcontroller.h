
#ifndef MRAPPLICATIONCONTROLLER_H
#define MRAPPLICATIONCONTROLLER_H

// INCLUDES
#include <QDeclarativeItem>
#include <qdeclarative.h>

// FORWARD DECLARATIONS
class QNetworkAccessManager;
class QNetworkReply;
class MrOpmlConfig;
class RSSChannelFile;
class MRApplicationController;
class MRApplicationControllerAttached;

// CLASS DECLARATIONS
class MRApplicationControllerAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MRApplicationController* controller READ controller CONSTANT)

public:
    explicit MRApplicationControllerAttached(QObject *parent = 0);
    ~MRApplicationControllerAttached();

    MRApplicationController *controller() const;    
    void setController(MRApplicationController *controller);

    void setNetworkAccessManager(QNetworkAccessManager *manager);



signals:
    void httpCompleted(uint requestId, QVariant data);
    void httpError();

public slots:
    void handleAppEvent();

    int httpGet(const QUrl &url);
    void httpCancel();
    // will called when view switch
    void cancelAllRssImageRequest();

private slots:
    void onHttpFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *mNwManager; // not owned
    MRApplicationController *mAppController; // not owned

    QMap<uint, QNetworkReply*> mNwReplyMap;
};

//
class MRApplicationController : public QDeclarativeItem
{
    Q_OBJECT

public:
    explicit   MRApplicationController(QDeclarativeItem *parent = 0);
    ~MRApplicationController();

    static MRApplicationControllerAttached *qmlAttachedProperties(QObject *object);

    static MRApplicationController* getController();


    Q_INVOKABLE static void appInitialize(bool aNeedReload = false);
    Q_INVOKABLE static MrOpmlConfig* opmlConfig();

    RSSChannelFile* channelFile(QString url, QString filepath = QString());



private:    
    Q_DISABLE_COPY(MRApplicationController)

    void clearResource();

    static MrOpmlConfig *mOpmlCfg;

    QMap<QString, RSSChannelFile*> mChannelsMap;
 };

QML_DECLARE_TYPEINFO(MRApplicationController, QML_HAS_ATTACHED_PROPERTIES)

#endif // MRAPPLICATIONCONTROLLER_H

// End of File
