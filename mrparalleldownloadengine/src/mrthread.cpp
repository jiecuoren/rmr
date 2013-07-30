#include <QNetworkAccessManager>
#include "mrthread.h"
#include "mrdebug.h"

MrThread::MrThread(QObject *parent) :
    QThread(parent),
    mNwkAccessMgr(NULL)
{
}

QNetworkAccessManager*MrThread::nwkAccessMgr()
{
    // mNwkAccessMgr should not be used in Main thread.
    // it is not necessary to lock mMutex here.
    return mNwkAccessMgr;
    // it is not necessary to Unlock mMutex here.
}

void MrThread::run()
{
    LOG_METHOD;

    Qt::HANDLE threadid = QThread::currentThreadId();
    LOG_FORMAT("  !!!thread MrThread::run id=%d",(int)threadid);

    mNwkAccessMgr = new QNetworkAccessManager;
    LOG_FORMAT("  in MrThread::run QNetworkAccessManager created thread id=%d",(int)threadid);

    emit nwkAccessMgrCreated();

    exec();

    delete mNwkAccessMgr;
    mNwkAccessMgr = NULL;
}
