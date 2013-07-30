#ifndef _IMAGE_MANAGER
#define _IMAGE_MANAGER

#include <QObject>
#include <QMap>
#include <QMutex>
#include <commonvalues.h>
#include <QThread>
class QImage;
class MrParallelDownloadEngine;
class MrParallelDownloadSession;

class ImageRequestToken : public QObject
{
    Q_OBJECT

public:
    QString mUrl;       //Input value
    QString mLocalpath; //Output value when succeed
    bool    mDone;      //Output value when done
    ImageType mType;

public:
    void done();
signals:
    void finished(QString);

    friend class ImageMgr;
};

class ImageMgr : public QObject
{
    Q_OBJECT

public:

    //inner class
    class PicInfo
    {
    public:
        QString mUrl;
        QString mLocalUrl;
        ImageType mType;
    };

public:
    static ImageMgr* openInstance();
    void closeInstance();

private:
    explicit ImageMgr(QObject* aParent = 0);
    ~ImageMgr();

public:
    Q_INVOKABLE void startDownLogo();
    Q_INVOKABLE void cancelAllRequest();

    QString getLocalPath(QString aUrl, ImageType aType);
    ImageRequestToken* addImageRequest( QString url,  ImageType aType );
    void cancelImageRequest( ImageRequestToken * token );

signals:
    void loadingFinished(QString aReturnStr, int aIndex);
    void loadingError(QString aErrorStr);
    void saveOriFinished();
    void cancel();

private slots:
    void picHandle(int aIndex, QByteArray aData, bool aFlag);
    void clipPic(const QImage& aImage,ImageType aType, int aIndex);
    void logoDownloadError(int aErrorCode);

private:
    QString combianString(QString aOriStr, QString aFixStr = "");
    bool checkIfDownload( const QString& aPath);
    void addRequest(int aIndex, const QString& aUrl);
    void clear();
    void initFolders();

private:
    static MrParallelDownloadEngine* mDownloadEngine;
    static MrParallelDownloadSession* mDownloadSession;
    QList<ImageRequestToken*> mRequestQueue;
    QString mImgPath;
    static QMutex mMutex;
    static int mRef;
    static ImageMgr* mInstance;
    static QThread* mThread;
};

#endif /* _IMAGE_MANAGER */
