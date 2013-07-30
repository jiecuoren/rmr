// INCLUDES
#include <QNetworkReply>
#include <QThread>//in order to LOG thread id
#include "mrdownloadrequest.h"
#include "mrdownloadrequesteventhandler.h"
#include "mrdebug.h"

//Implementation
MrDownloadRequest::MrDownloadRequest(const int &aIndex,
                                     const QString &aUrl,
                                     MrDownloadRequestEventHandler   *aEventHandler)
    : mIndex(aIndex),mUrl(aUrl),mReply(NULL),mEventHandler(aEventHandler)
{
}

void MrDownloadRequest::onRequestFinished()
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrDownloadRequest::onRequestFinished id=%d",(int)threadid);

    //MrParallelDownloadSession is MrDownloadRequestEventHandler.
    //MrDownloadRequest can call MrParallelDownloadSession::processFinishedReq.
    mEventHandler->processFinishedReq(this);
}

int MrDownloadRequest::index() const
{
    return mIndex;
}

QString MrDownloadRequest::url() const
{
    return mUrl;
}

void MrDownloadRequest::setReply(QNetworkReply *aReply)
{
    if(NULL != aReply)
    {
        mReply = aReply;
    }
}

QNetworkReply*  MrDownloadRequest::reply() const
{
    return mReply;
}

// End of File
