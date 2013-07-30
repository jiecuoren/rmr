#ifndef IRPARALLELDOWNLOADENGINE_H
#define IRPARALLELDOWNLOADENGINE_H

#include <QObject>

class MrParallelDownloadSession;
class MrThread;

class MrParallelDownloadEngine : public QObject
{
    Q_OBJECT

public:
    /**
     *  Get the instance of IRParallelDownloadEngine
     *  @return IRParallelDownloadEngine*
     */
    static  MrParallelDownloadEngine* openInstance();

    /**
     *  Close the instance of IRParallelDownloadEngine
     */
    void closeInstance();

    /**
     *  Interface for UI
     *  get a session object,in order to send request via session
     */
    MrParallelDownloadSession* createSession();

    /**
     *  Interface for UI
     *  delete a session object when no longer use it. Call by UI or ~IRParallelDownloadEngine().
     *  @@@@@@@@@@@@@@@
     *  @@ parameters @
     *  @@@@@@@@@@@@@@@
     *  @ aSession  : the session pointer need to close. Provided by UI.
     *  @ aCloseAll : this is used in  ~IRParallelDownloadEngine(), not for UI.
     *                UI should keep default value of this parameter or set it as false.
     */
    void closeSession(MrParallelDownloadSession *aSession,bool aCloseAll=false);

signals:
    /**
     *  signal for MrThread
     *  if you want to terminate a thread , send this signal
     */
    void threadQuit();

private:
    MrParallelDownloadEngine();
    ~MrParallelDownloadEngine();

    void adjustAllowedCount();

    int                                     mSessionCount;
    QList<MrParallelDownloadSession*>       mSessionList;

    int                                     mRefCount;
    static MrParallelDownloadEngine        *mInstance;

    /*a list of threads*/
    QList<MrThread*>                        mThreadList;
};
#endif

