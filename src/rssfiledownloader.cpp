#include <QDebug>
#include <QThread>
#include <QtAlgorithms>
#include <QApplication>

#include "commonvalues.h"
#include "mrparalleldownloadengine.h"
#include "mrparalleldownloadsession.h"
#include "rssfiledownloader.h"
#include <mrdebug.h>

RSSFileDownloader* RSSFileDownloader::mInstance = NULL;
int RSSFileDownloader::mRef = 0;
QMutex RSSFileDownloader::mMutex;

RSSFileDownloader* RSSFileDownloader::openInstance()
{

    LOG_METHOD;
    mMutex.lock();

    if(NULL == mInstance)
    {
        mInstance = new RSSFileDownloader;
    }

    ++mRef;

    mMutex.unlock();

    return mInstance;
}

void RSSFileDownloader::closeInstance()
{
    LOG_METHOD;

    mMutex.lock();

    if(NULL != mInstance)
    {
        --mRef;

        if(0 == mRef)
        {
            mInstance->disconnect();
            delete mInstance;
            mInstance = NULL;
        }

    }

    mMutex.unlock();

    return;
}

RSSFileDownloader::RSSFileDownloader(QObject *aParent) : QObject(aParent),
    mDownloadEngine(NULL), mDownloadSession(NULL)
{

}

RSSFileDownloader::~RSSFileDownloader()
{
    LOG_METHOD;
    if (mDownloadSession)
    {
        mDownloadSession->cancelAllRequests();
    }

    if (mDownloadEngine)
    {
        mDownloadEngine->closeInstance();
        mDownloadEngine = NULL;
    }

    clear();
}

RSSRequest* RSSFileDownloader::getRSSFile(const QString& aUrl)
{

    LOG_METHOD;

    if(aUrl.isEmpty())
    {
        return NULL;
    }


    RSSRequest* tempRequest = new RSSRequest;
    addRequest(mRequestList.count(), aUrl);
    mRequestList.append(tempRequest);

    return tempRequest;
}

void RSSFileDownloader::addRequest(int aIndex, QString aUrl)
{
    LOG_METHOD;

    if (NULL == mDownloadEngine)
    {
        mDownloadEngine = MrParallelDownloadEngine::openInstance();
    }

    if (NULL == mDownloadSession)
    {
        mDownloadSession = mDownloadEngine->createSession();
        connect(mDownloadSession, SIGNAL(downloadFinished(int, QByteArray, bool)),
                this, SLOT(requestFinished(int, QByteArray, bool)));
        connect(mDownloadSession, SIGNAL(downloadError(int)),
                this, SLOT(requestError(int)));
        connect(mDownloadSession, SIGNAL(downloadCancel()),
                this, SIGNAL(requestCancel()));
        connect(mDownloadSession, SIGNAL(downloadRedirect(int, QString)),
                this, SLOT(onDownloadRedirect(int, QString)));
    }

    if(NULL != mDownloadSession)
    {

        //qDebug() << "RSSFileDownloader::addRequest() thread id is " << QThread::currentThreadId();
        mDownloadSession->addRequest(aIndex,  aUrl);
        startDownloading();
    }
}

void RSSFileDownloader::startDownloading()
{

    LOG_METHOD;

    if(NULL != mDownloadSession)
    {
        mDownloadSession->startSchedule();
    }
}

void RSSFileDownloader::requestFinished(int aIndex, QByteArray aData, bool aFlag)
{
    Q_UNUSED(aFlag)

    LOG_METHOD;
    //qDebug() << "finished data is " << QString(aData);
    QFile myFile("c:/data1");
    myFile.open(QFile::ReadWrite | QFile::Text);
    QTextStream out(&myFile);
    out << aData;
    myFile.close();

//    LOG_FORMAT("finish data is %s", QString(aData));
    //qDebug()<<"RSSFileDownloader::requestFinished thread :"<<QThread::currentThreadId();
    RSSRequest* tempRequest = mRequestList.at(aIndex);
    //qDebug() << "emit tempRequest->finished(aData); before " << QString(aData);
    emit tempRequest->finished(aData);
}

void RSSFileDownloader::onDownloadRedirect(int aIndex, QString aUrl)
{
    LOG_METHOD;
    addRequest(aIndex, aUrl);
}

void RSSFileDownloader::requestError(int aIndex)
{
    //qDebug() << "RSSFileDownloader::requestError()";
    //qDebug() << "requestError code is " << aIndex;
}

void RSSFileDownloader::clear()
{
    //qDebug() << "RSSFileDownloader::clear()";

    qDeleteAll(mRequestList.begin(), mRequestList.end());

    mRequestList.clear();
}

void RSSFileDownloader::cancelAllRequest()
{
    //qDebug() << "RSSFileDownloader::cancelAllRequest()";
    if(NULL != mDownloadSession)
    {
        mDownloadSession->cancelAllRequests();
    }

    clear();
}
