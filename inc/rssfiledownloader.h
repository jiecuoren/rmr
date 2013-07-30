#ifndef _RSS_FILE_DOWNLOADER_
#define _RSS_FILE_DOWNLOADER_

#include <QObject>
#include <QMutex>

class MrParallelDownloadEngine;
class MrParallelDownloadSession;

class RSSRequest : public QObject
{
    Q_OBJECT
public:
    RSSRequest() {}

signals:
    void finished(QByteArray aData);
//    void redirect(int aIndex, QString aUrl);
    friend class RSSFileDownloader;
};

class RSSFileDownloader : public QObject
{
    Q_OBJECT
public:
    static RSSFileDownloader* openInstance();
    void closeInstance();
    void cancelAllRequest();
private:
    RSSFileDownloader(QObject* aParent = NULL);
    ~RSSFileDownloader();
    void clear();
    void addRequest(int aIndex, QString aUrl);

    void startDownloading();
public:
    RSSRequest* getRSSFile(const QString& aUrl);

signals:
    void requestCancel();

private slots:
    void requestFinished(int aIndex, QByteArray aData, bool aFlag);
    void requestError(int aIndex);
    void onDownloadRedirect(int aIndex, QString aUrl);
private:
    QList<RSSRequest* > mRequestList;
    static QMutex mMutex;
    static int mRef;
    static RSSFileDownloader* mInstance;
    MrParallelDownloadEngine* mDownloadEngine;
    MrParallelDownloadSession* mDownloadSession;
};

#endif /* _RSS_FILE_DOWNLOADER_ */
