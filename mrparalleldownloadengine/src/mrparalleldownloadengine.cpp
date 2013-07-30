#include "mrparalleldownloadengine.h"
#include "mrdebug.h"
#include "mrparalleldownloadsession.h"
#include "mrthread.h"


// Consts
const int KDownloadingReqMax = 4;//Max value of Downloading Requests's sum in all session
const int KSessionCountMax = 3;//Max value of total session count

//Implementation
MrParallelDownloadEngine *MrParallelDownloadEngine::mInstance = NULL;

MrParallelDownloadEngine* MrParallelDownloadEngine::openInstance()
{
    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadEngine::openInstance=%d",(int)threadid);

    if (NULL == mInstance)
    {
        mInstance = new MrParallelDownloadEngine();
    }
    else
    {
        mInstance->mRefCount++;
    }

    return mInstance;
}

void MrParallelDownloadEngine::closeInstance()
{
    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadEngine::closeInstance=%d",(int)threadid);

    if ((--mRefCount) == 0)
    {
        if(this == mInstance)
        {
            mInstance = NULL;
        }
        delete this;
    }
}

MrParallelDownloadEngine::MrParallelDownloadEngine()
    :mSessionCount(0),mRefCount(1)
{
    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadEngine::MrParallelDownloadEngine=%d",(int)threadid);
}

MrParallelDownloadEngine::~MrParallelDownloadEngine()
{
    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadEngine::~MrParallelDownloadEngine id=%d",(int)threadid);

    //close All session;//////////////////////////////////////////
    //can't use foreach; remove object in loop is not safe.
    foreach (MrParallelDownloadSession *sessionInList, mSessionList)
    {
        closeSession(sessionInList,true);
    }
}

MrParallelDownloadSession* MrParallelDownloadEngine::createSession()
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadEngine::createSession id=%d",(int)threadid);

    MrParallelDownloadSession *sessionRet = NULL;
    MrThread *thread = NULL;

    //use this Macro, control the rule all in one place without another const
    if(mSessionList.count() < KSessionCountMax)
    {
        sessionRet = new MrParallelDownloadSession();



        if(NULL != sessionRet)
        {
            thread = new MrThread(this);
            connect(thread, SIGNAL(nwkAccessMgrCreated()), sessionRet, SLOT(onThreadCreated()));
            connect(this, SIGNAL(threadQuit()), thread, SLOT(quit()));
            thread->start();

            mSessionList.append(sessionRet);
            mThreadList.append(thread);

            //Note:  "new QThread" is moved to ctor avoid NULL AccessMgr pointer
            sessionRet->moveToThread(thread);
            //Note: "start() thread" is moved to ctor avoid NULL AccessMgr pointer

        }
        else
        {
            return NULL;
        }
    }

    //ajuest MaxActiveCnt of all sessions in mSessionList according rule
    adjustAllowedCount();//using mSessionCount

    return sessionRet;
}

void MrParallelDownloadEngine::closeSession(MrParallelDownloadSession *aSession,bool aCloseAll)
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadEngine::closeSession id=%d",(int)threadid);

    if(NULL == aSession)
    {
        return;//if NULL,  not necessary to change MaxActiveCount
    }

    aSession->stopSchedule();////////////////////////////////////////////////////////////
    aSession->cancelAllRequests();//////////////////////////////////////////////////////

    emit threadQuit();
    MrThread* mrThread = aSession->mrThread();
    mrThread->wait();
    mThreadList.removeOne(mrThread);

    if(!aCloseAll)//if closeSession() is called in destructor,aCloseAll is true,removal form mSessionList is not necessary.
    {
        mSessionList.removeOne(aSession);//can not assgin NULL before remove form SessionList

        //ajuest MaxActiveCnt of all sessions in mSessionList according rule.
        //can't call it in destructor,because some other session deleted just now is still in list, pointer is wild.
        adjustAllowedCount();
    }

    delete aSession;
    aSession=NULL;
}

void MrParallelDownloadEngine::adjustAllowedCount()
{
    LOG_METHOD;
    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrParallelDownloadEngine::adjustAllowedCount id =%d",(int)threadid);

    if (mSessionList.count() <= 0)
        return;

    int maxAllowedCount = 0;//this is modified later, and set to sessions

    maxAllowedCount = KDownloadingReqMax/mSessionList.count();

    LOG_FORMAT("   maxAllowedCount  =%d",maxAllowedCount);

    //ajuest MaxActiveCnt of all sessions in mSessionList
    foreach (MrParallelDownloadSession *sessionInList, mSessionList)
    {
        sessionInList->setMaxActiveCount(maxAllowedCount);
    }
}

// End of File
