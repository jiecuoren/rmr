
// INCLUDES
#include <QDeclarativeEngine>
#include <QNetworkReply>
#include <QDateTime>
#include <imagemgr.h>

#include "mrapplicationcontroller.h"
#include "rsschannelfile.h"
#include "mrdebug.h"
#include "mropmlconfig.h"

// IMPLEMENTATIONS
///////////////////////////////////////////////////////////////////////////////
class MRApplicationControllerPrivate
{
public:
    static MRApplicationController *q_ptr;
};

MRApplicationController *MRApplicationControllerPrivate::q_ptr = NULL;

///////////////////////////////////////////////////////////////////////////////
MRApplicationControllerAttached::MRApplicationControllerAttached(QObject *parent)
    :QObject(parent),
    mNwManager(NULL),
    mAppController(NULL)
{
    LOG_METHOD;    
}

MRApplicationControllerAttached::~MRApplicationControllerAttached()
{
    LOG_METHOD;

    foreach (QNetworkReply *reply, mNwReplyMap)
    {
        if (reply)
        {
            delete reply;
            reply = NULL;
        }
    }

    mNwReplyMap.clear();    
}

MRApplicationController *MRApplicationControllerAttached::controller() const
{
    LOG_METHOD;

    return mAppController;
}

void MRApplicationControllerAttached::setController(MRApplicationController *controller)
{
    LOG_METHOD;

    mAppController = controller;
}

void MRApplicationControllerAttached::setNetworkAccessManager(QNetworkAccessManager *manager)
{
    LOG_METHOD;

    if (mNwManager)
    {
        disconnect(mNwManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onHttpFinished(QNetworkReply*)));
    }

    mNwManager = manager;
    connect(mNwManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onHttpFinished(QNetworkReply*)));
}

void MRApplicationControllerAttached::handleAppEvent(){
    LOG_METHOD;
}


int MRApplicationControllerAttached::httpGet(const QUrl &url)
{
    LOG_METHOD;
    LOG(url.toString());

    if (!mNwManager)
        return -1;

    QNetworkReply *reply = mNwManager->get(QNetworkRequest(url));
    uint replyId = QDateTime::currentDateTime().toTime_t();
    if (mNwReplyMap.contains(replyId))
    {
        replyId = QDateTime::currentDateTime().addDays(-1).toTime_t();
    }
    mNwReplyMap.insert(replyId, reply);

    return replyId;
}

void MRApplicationControllerAttached::httpCancel()
{
    LOG_METHOD;

    foreach (QNetworkReply *reply, mNwReplyMap)
    {
        if (reply)
        {
            reply->close();
        }
    }
}

void MRApplicationControllerAttached::onHttpFinished(QNetworkReply *reply)
{
    LOG_METHOD;

    uint key = mNwReplyMap.key(reply);

    if (QNetworkReply::NoError == reply->error())
    {
        if (reply->isOpen())
        {
            LOG("Http request completd");
            QByteArray ba(reply->readAll());
            const char *c_str2 = ba.data();

            emit httpCompleted(key, QString::fromUtf8(c_str2));
        }
    }
    else
    {
        LOG_FORMAT("Http request error = %d", reply->error());
        emit httpError();
    }

    mNwReplyMap.remove(key);
    reply->deleteLater();
    reply = NULL;
}


///////////////////////////////////////////////////////////////////////////////

MrOpmlConfig *MRApplicationController::mOpmlCfg = NULL;

MRApplicationController::MRApplicationController(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    LOG_METHOD;

    MRApplicationControllerPrivate::q_ptr = this;

}

MRApplicationController* MRApplicationController::getController()
{
    return MRApplicationControllerPrivate::q_ptr;
}



void MRApplicationController::appInitialize(bool aNeedReload)
{
    LOG_METHOD;

    if(aNeedReload)
    {
        if(NULL != mOpmlCfg)
        {
            mOpmlCfg->closeInstance();
        }
        mOpmlCfg = MrOpmlConfig::openInstance();
    }
    else
    {
        if (NULL == mOpmlCfg)
            mOpmlCfg = MrOpmlConfig::openInstance();
    }

}

MrOpmlConfig* MRApplicationController::opmlConfig()
{
    return MrOpmlConfig::openInstance();
}

MRApplicationController::~MRApplicationController()
{
    LOG_METHOD;
    mOpmlCfg->closeInstance();
    clearResource();
}

MRApplicationControllerAttached *MRApplicationController::qmlAttachedProperties(QObject *object)
{
    LOG_METHOD;

    MRApplicationControllerAttached *attached = new MRApplicationControllerAttached(object);
    attached->setController(MRApplicationControllerPrivate::q_ptr);
    appInitialize();

//    QDeclarativeEngine *engine = qmlEngine(object);

//    if (engine)
//    {
//        LOG("MRApplicationController::qmlAttachedProperties set network engine to attachment object");
//        attached->setNetworkAccessManager(engine->networkAccessManager());
//    }

    return attached;
}

RSSChannelFile* MRApplicationController::channelFile(QString url, QString filepath)
{
    if (mChannelsMap.contains(url)){
        return mChannelsMap.value(url);
    }else{
        RSSChannelFile* newChannel = new RSSChannelFile(this);
        newChannel->setSourceUrl(url, filepath);
        mChannelsMap.insert(url, newChannel);

        return newChannel;
    }
}

void MRApplicationController::clearResource()
{
    mChannelsMap.clear();
}

void MRApplicationControllerAttached::cancelAllRssImageRequest()
{

    LOG_METHOD;

    // cancel all rss xml request
    RSSChannelFile tempChannel;
    tempChannel.cancelAllRequest();

    // cancel all image request
    ImageMgr* tempImgMgr = ImageMgr::openInstance();
    tempImgMgr->cancelAllRequest();
    tempImgMgr->closeInstance();
}


// End of File
