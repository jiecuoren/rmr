// INCLUDES
#include <QNetworkReply>
#include <QTimer>// for QTimer::singleShot()
#include <QMutex>
#include "mrparalleldownloadsession.h"
#include "mrdebug.h"
#include "mrdownloadrequest.h"
#include "mrthread.h"


// Consts
const int KRquestCountMax = 200;

//Implementation
MrParallelDownloadSession::MrParallelDownloadSession()
                          :mMaxActiveCount(0),
                           mActiveCount(0),
                           mStarted(false),
                           mThread(NULL),
                           mPendingRequest(false)
{
      Qt::HANDLE threadid = QThread::currentThreadId();
      LOG_FORMAT("  !!!thread MrParallelDownloadSession::MrParallelDownloadSession id=%d",(int)threadid);

      mMutex = new QMutex(QMutex::NonRecursive);

      mNwkAccessMgr = NULL;
}

MrParallelDownloadSession::~MrParallelDownloadSession()
{
    LOG_METHOD;
    delete mMutex;
}

//Interface for UI
//receive a index and url, create a MrDownloadRequest and append it to mReqMap
//then, schedule the downloading
bool MrParallelDownloadSession::addRequest(const int &aIndex,const QString &aUrl)
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadSession::addRequest id=%d",(int)threadid);

    mMutex->lock();

    bool ret = true;

    if(KRquestCountMax > mReqMap.count())
    {
        //construct request object according to input parameter
        MrDownloadRequest *request = new MrDownloadRequest(aIndex,aUrl,this);

        request->moveToThread(this->thread());//in order to let onRequestFinished() run in sub thread

        //append just creaded request to mReqMap
        //This is not QList shouldn't use append();
        mReqMap.insert(aIndex, request);
    }
    else
    {
        ret = false;
    }

    mMutex->unlock();
    return ret;

    // can not scheduleReqForDownloading() now,  shedule it in startSchedule().
}

//aIndexs need Traversall from start to end, so use foreach
void MrParallelDownloadSession::removeRequest(const QList<int> &aIndexList)
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadSession::removeRequest_QList id=%d",(int)threadid);

    //NonRecursive Lock.  locked in removeRequest(inputIndex),  mMutex.lock() is not necessary here.

    //foreach is OK here. loop index by aIndexList, but the removed container is mReqMap. It's Safe!!
    foreach (int inputIndex, aIndexList)
    {
       removeRequest(inputIndex);
    }

    //NonRecursive Lock.  locked in removeRequest(inputIndex),  mMutex.unLock() is not necessary here.
}

void MrParallelDownloadSession::removeRequest(const int &aIndex)
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadSession::removeRequest id=%d",(int)threadid);

    mMutex->lock();

    //check exist before using"value",avoid insert default item into map when item is not contains in the map
    if(mReqMap.contains(aIndex))
    {
        MrDownloadRequest *request = mReqMap.value(aIndex);
        if(aIndex == request->index())
        {
            QNetworkReply *reply = request->reply();

            if(NULL != reply)
            {
                //cancel Active downloading req
                //can't call "reply->abort()" directly, consider finished request.
//                reply->isFinished()?reply->close():reply->abort();
                reply->close();
                reply->deleteLater();
                reply = NULL;
            }

            //delete request;
            request->deleteLater();

            //remove request from requestList
            mReqMap.remove(aIndex);
        }
    }

    mMutex->unlock();
}

void MrParallelDownloadSession::startSchedule()
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadSession::startSchedule id=%d",(int)threadid);

    mMutex->lock();

    mStarted = true;

    mMutex->unlock();

    if(mNwkAccessMgr != NULL)
    {
        QTimer::singleShot(0, this, SLOT(scheduleReqForDownloading()));
    }
}

void MrParallelDownloadSession::stopSchedule()
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadSession::stopSchedule id=%d",(int)threadid);


    mMutex->lock();

    mStarted = false;

    mMutex->unlock();
}

//remove All MrDownloadRequest objects in mReqMap
void MrParallelDownloadSession::cancelAllRequests()
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadSession::cancelAllRequests id=%d",(int)threadid);

    mMutex->lock();

    //have to remove element according to index of QMap, can not use foreach
    QMapIterator<int, MrDownloadRequest *> i(mReqMap);

    while (i.hasNext())
    {
        i.next();

        MrDownloadRequest *request = i.value();

        QNetworkReply *reply = request->reply();
        //remove is not necessary; so index is not necessary.

        if(NULL != reply)
        {
            //cancel Active downloading req
            //have to be cancle here,
            //otherwise if next enter event loop, reply notified,
            //but request object is already deleted,crash
            //can't call "reply->abort()" directly, consider finished request.
//            bool isFinished = reply->isFinished();
//            isFinished ? reply->close():reply->abort();
            reply->close();
            reply->deleteLater();
            reply = NULL;
        }

        request->deleteLater();

        //remove request from requestList is not necessary;clear all one time at the end of function
        //removeAt is faster than removeOne(T)
        //can not use qDeleteAll, otherwise QList is Traversall again.
    }//end of while

    mReqMap.clear();
    mActiveCount = 0;//rest downloading count
    mMutex->unlock();
    emit downloadCancel();
}

void MrParallelDownloadSession::setMaxActiveCount(int aMaxCount)
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadSession::setMaxActiveCount id=%d",(int)threadid);

    mMutex->lock();

    mMaxActiveCount = aMaxCount;

    mMutex->unlock();
}

//Called by MrDownloadRequest in its requestFinished() slot
void MrParallelDownloadSession::processFinishedReq(MrDownloadRequest *aRequest)
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadSession::processFinishedReq id=%d",(int)threadid);

    //output log
    const int indexFinished = aRequest->index();
    LOG_FORMAT("  indexFinished =%d",indexFinished);

    //get finished Reply object
    QNetworkReply *tempReply = aRequest->reply();

    //output log
    const QString urlFinished = tempReply->url().toString();//const
    LOG_FORMAT(" Finished url  =%s",urlFinished.toLocal8Bit().constData());
    LOG_FORMAT(" url in req  =%s",aRequest->url().toLocal8Bit().constData());

    //Notify
    if (QNetworkReply::NoError != tempReply->error())
    {
        LOG_FORMAT("  error code=%s",STRING2CHAR(tempReply->errorString()));
        emit downloadError(indexFinished);
    }
    else
    {
        QUrl tempUrl = tempReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        if(tempUrl.isValid())
        {
            LOG("Yes, redirect : " + tempUrl.toString());
            emit downloadRedirect(indexFinished, tempUrl.toString());
        }
        else
        {
            emit downloadFinished(indexFinished,tempReply->readAll(),false);
        }

    }

    //reply is useless; it is created by QNAM in this class , so delete it in this class
    tempReply->deleteLater();
    tempReply = NULL;

    aRequest->deleteLater();//aRequest has no parent.

    mMutex->lock();
    mReqMap.remove(indexFinished);//since we've know the index, this is faster than mReqMap.removeOne(aRequest);
    LOG( "[[removed]]" );
    //this is important, otherwise if reqs are more than 4, they can't be processed
    mActiveCount--;//be pair in scheduleReqForDownloading(),  "mActiveCount++;"
    mMutex->unlock();

    //shedule downloading list, it will send req while Active<Max
    if(mStarted)
    {
        QTimer::singleShot(0, this, SLOT(scheduleReqForDownloading()));
    }

}

//if mActiveCount<mMaxActiveCount, find the un-sent request, then send it
void MrParallelDownloadSession::scheduleReqForDownloading()
{
    LOG_METHOD;
    
    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadSession::scheduleReqForDownloading id=%d",(int)threadid);

    mMutex->lock();

    //MrParallelDownloadEngine *engine = qobject_cast<MrParallelDownloadEngine*>(parent());

    QMapIterator<int, MrDownloadRequest *> i(mReqMap);

    mMutex->unlock();

    while (i.hasNext()&& (mActiveCount<mMaxActiveCount) )
    {

        //IO can't lock with mMutex
        LOG_FORMAT("    ++mActiveCount   [Before =%d",mActiveCount);
        LOG_FORMAT("    ++mMaxActiveCount = %d",mMaxActiveCount);
        LOG_FORMAT("    ++mReqList Count [Before= %d",mReqMap.count());
        //IO can't lock with mMutex

        i.next();
        MrDownloadRequest *requestForSchedule = i.value();

        //if reply is not set in MrDownloadRequest, this Request hasn't been send to QNAM.
        //get the url with QNAM now.
        if(NULL == requestForSchedule->reply())
        {
            QString urlForDownload = requestForSchedule->url();
            LOG_FORMAT("   url to download is %s", STRING2CHAR(urlForDownload));


                QUrl url(urlForDownload);

                LOG_FORMAT( "  (( )) before get , mNwkAccessMgr=%d Tid=%d",(int)mNwkAccessMgr,(int)threadid );

                //QNwkAccessMgr::get is slow, don't lock mMutex here. mNwkAccessMgr is not used in main thread.
                QNetworkReply *tempReply = NULL;
                if(mNwkAccessMgr != NULL)
                {
                    tempReply = mNwkAccessMgr->get(QNetworkRequest(url));
                }
                //QNwkAccessMgr::get is slow, don't lock mMutex here. mNwkAccessMgr is not used in main thread.

                LOG( "  (( )) after get" );

                //signal is connected to MrDownloadRequest
                connect(tempReply, SIGNAL(finished()),
                        requestForSchedule, SLOT(onRequestFinished()));

                //set QNetworkReply into MrDownloadRequest
                requestForSchedule->setReply(tempReply);

                mMutex->lock();
                mActiveCount++;//increase Active count
                               ////be pair in processFinishedReq(),  "mActiveCount--;"
                mMutex->unlock();
        }//end of if(NULL == requestForSchedule->reply())
    }//end of while
}//end of function

void MrParallelDownloadSession::setNwkAccessMgr(QNetworkAccessManager * aNwkAccessMgr)
{
      LOG_METHOD;
      Qt::HANDLE threadid = QThread::currentThreadId();
      LOG_FORMAT("  !!!thread MrParallelDownloadSession::setNwkAccessMgr id=%d",(int)threadid);

      mNwkAccessMgr = aNwkAccessMgr;
}

void MrParallelDownloadSession::scheduleWhenAccessMgrReady()
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadSession::scheduleWhenAccessMgrReady id=%d",(int)threadid);

    if((mNwkAccessMgr != NULL) && (mStarted))
    {
        QTimer::singleShot(0, this, SLOT(scheduleReqForDownloading()));
    }

}

void MrParallelDownloadSession::onThreadCreated()
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadSession::onThreadCreated id=%d",(int)threadid);

    MrThread* thread = qobject_cast<MrThread*>(sender());
    mThread = thread;
    QNetworkAccessManager* nwkAcsMgrInThread = thread->nwkAccessMgr();

    LOG_FORMAT("$$$$$$ mNwkAccessMgr in thread = %d $$$$",(int)nwkAcsMgrInThread);
    setNwkAccessMgr(nwkAcsMgrInThread);//aNwkAcsMgrInThread
    scheduleWhenAccessMgrReady();
}

MrThread* MrParallelDownloadSession::mrThread()
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadSession::MrThread id=%d",(int)threadid);

    return mThread;
}

// End of File
