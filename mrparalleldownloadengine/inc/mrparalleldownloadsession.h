#ifndef MrParallelDownloadSession_H
#define MrParallelDownloadSession_H

#include <QObject>
#include <QMap>
#include "mrdownloadrequesteventhandler.h"
//#include "mrthread.h"

class QNetworkAccessManager;
class QMutex;
class MrDownloadRequest;
class MrParallelDownloadEngine;
class MrThread;

class  MrParallelDownloadSession : public QObject, public MrDownloadRequestEventHandler
{
    Q_OBJECT

public:
    bool addRequest(const int &aIndex,const QString &aUrl);
    void removeRequest(const QList<int> &aIndexList);
    void removeRequest(const int &aIndex);
    void startSchedule();
    void stopSchedule();
    void cancelAllRequests();
    
    MrThread* mrThread();
    
signals:
    void downloadFinished(const int &aIndex ,const QByteArray &aLogoData, bool aIsinDB);
    void downloadError(const int &aIndex);
    void downloadCancel();
    void downloadRedirect(const int aIndex, const QString& aReUrl);

private slots:
    void scheduleReqForDownloading();
    void onThreadCreated();

private:
    explicit MrParallelDownloadSession();
    ~MrParallelDownloadSession();

    void setMaxActiveCount(int aMaxCount);//call by engine
    void processFinishedReq(MrDownloadRequest *aRequest);//Nwk finishe callback, call by MrDownloadRequest

    void setNwkAccessMgr(QNetworkAccessManager * aNwkAccessMgr);//call by Session::onThreadCreated()
    void scheduleWhenAccessMgrReady();//re-Schedule if NwkAccessMgr is not created when startSchedule() called
    
private:
    int                            mMaxActiveCount;
    int                            mActiveCount;
    QMap<int,MrDownloadRequest*>   mReqMap;
    bool                           mStarted;

    QMutex                        *mMutex;
    QNetworkAccessManager         *mNwkAccessMgr;

    MrThread                      *mThread;
    bool                           mPendingRequest;
    
    friend class MrParallelDownloadEngine;//in order to call setMaxActiveCount()
};
#endif
