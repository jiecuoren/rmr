#include <QDebug>
#include <QGraphicsObject>
#include <QImage>
#include <QApplication>
#include <QFile>
#include <QMap>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtAlgorithms>
#include <QDir>
#include "mrdebug.h"
#include "imagemgr.h"
#include "commonvalues.h"
//#include "dataprovider.h"
#include "mrparalleldownloadengine.h"
#include "mrparalleldownloadsession.h"

QMutex ImageMgr::mMutex;
int ImageMgr::mRef = 0;
ImageMgr* ImageMgr::mInstance = NULL;
QThread* ImageMgr::mThread = NULL;
MrParallelDownloadEngine* ImageMgr::mDownloadEngine = NULL;
MrParallelDownloadSession* ImageMgr::mDownloadSession = NULL;

ImageMgr* ImageMgr::openInstance()
{
    mMutex.lock();

    if(NULL == mInstance)
    {
        mInstance = new ImageMgr;
        mThread = new QThread;
        mInstance->moveToThread(mThread);

        if (NULL == mDownloadEngine)
        {
            mDownloadEngine = MrParallelDownloadEngine::openInstance();
        }

        if (NULL == mDownloadSession)
        {
            mDownloadSession = mDownloadEngine->createSession();
            connect(mDownloadSession, SIGNAL(downloadFinished(int, QByteArray, bool)),
                    mInstance, SLOT(picHandle(int, QByteArray, bool)));
            connect(mDownloadSession, SIGNAL(downloadError(int)),
                    mInstance, SLOT(logoDownloadError(int)));
        }

        mThread->start();
        mThread->setPriority(QThread::LowPriority);
    }

    ++mRef;

    mMutex.unlock();

    return mInstance;
}

void ImageMgr::closeInstance()
{
    mMutex.lock();

    if(NULL != mInstance)
    {
        --mRef;
        if(0 == mRef)
        {

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

            mInstance->disconnect();

            mInstance->moveToThread(QApplication::instance()->thread());

            mThread->terminate();
            delete mThread;
            mThread = NULL;
			

            delete mInstance;
            mInstance = NULL;
            
        }
    }

    mMutex.unlock();
}

ImageMgr::ImageMgr(QObject *aParent): QObject(aParent), mImgPath("")
{
    initFolders();
}

ImageMgr::~ImageMgr()
{

}

QString ImageMgr::getLocalPath(QString aUrl, ImageType aType)
{
    LOG_METHOD;

    QString ret = "";

    if(!aUrl.isEmpty())
    {
        QString postFix = "";

        switch (aType)
        {
        case EIcon :
            postFix = KPostfixIcon;
            break;
        case E150150 :
            postFix = KPostfixClip150150;
            break;
        case E320150 :
            postFix = KPostfixClip320150;
            break;
        }

        QString tempLocalPath = combianString(aUrl, postFix);

        if(checkIfDownload(tempLocalPath ))
        {
            ret = tempLocalPath;
        }

    }

    return ret;

}

QString ImageMgr::combianString(QString aOriStr, QString aFixStr)
{
    LOG_METHOD;

    if(aOriStr.isEmpty() || aOriStr.isEmpty())
    {
        return "";
    }

    QString fixStr = ".jpg";
    QString tempUrl = aOriStr;
    QString fileName = QString(tempUrl.toAscii().toBase64()).remove("/").remove("+") + aFixStr + fixStr;

    return QString(mImgPath + "/" + fileName);
}

bool ImageMgr::checkIfDownload(const QString& aPath)
{
    bool ret = false;

    if(QFile::exists(aPath))
    {
        ret = true;
    }

    return ret;
}

// we save every original pic anyway
void ImageMgr::picHandle(int aIndex, QByteArray aData, bool aFlag)
{
    Q_UNUSED(aFlag)

    LOG_METHOD;

    //qDebug() << "picHandle's thread pri is:" << QThread::currentThread()->priority();

    //qDebug() << "ImageMgr::picHandle aIndex is " << aIndex;
    //qDebug()<<"ImageMgr thread in picHandle:"<<QThread::currentThreadId();
    if(aData.isEmpty())
    {
        //qDebug() << "ImageMgr::picHandle, aData.isEmpty()";
        return;
    }

    QImage originalImg = QImage::fromData(aData);

    if(aIndex>=mRequestQueue.count())
    {
        return;
    }

    ImageRequestToken* tempToken = mRequestQueue.at(aIndex);
    originalImg.save(combianString( tempToken->mUrl, ""));

    //downloaded, and saved, we do clip next
    switch (tempToken->mType)
    {
    case EIcon:
        {
            //qDebug() << "ImageMgr::picHandle  EIcon";
            //qDebug() << "tempToken->mLocalpath" << tempToken->mLocalpath;
            if(!checkIfDownload(tempToken->mLocalpath))
            {
                //qDebug() << "!checkIfDownload(tempToken->mLocalpath)";
                clipPic(QImage::fromData(aData), EIcon, aIndex);
            }
            else
            {
                emit tempToken->finished(tempToken->mLocalpath);
            }
            break;
        }

    case EOriPic:
        {
            //qDebug() << "ImageMgr::picHandle  EOriPic";
            //if we need ori pic, just return the url
            emit tempToken->finished(tempToken->mLocalpath);
            break;
        }

    case E150150:
        {
            //qDebug() << "ImageMgr::picHandle  E150150";
            if(!checkIfDownload(tempToken->mLocalpath))
            {
                clipPic(QImage::fromData(aData), E150150, aIndex);
            }
            else
            {
                emit tempToken->finished(tempToken->mLocalpath);
            }
            break;
        }
    case E320150:
        {
            //qDebug() << "ImageMgr::picHandle  E320150";
            if(!checkIfDownload(tempToken->mLocalpath))
            {
                clipPic(QImage::fromData(aData),E320150, aIndex);
            }
            else
            {
                emit tempToken->finished(tempToken->mLocalpath);
            }
            break;
        }
    default:
        break;

    }
}

void ImageMgr::clipPic(const QImage& aImage,ImageType aType, int aIndex)
{
    LOG_METHOD;

    //qDebug() << "ImageMgr::clipPic aIndex is " << aIndex;

    QSize imgSize = aImage.size();
    int imgWidth = imgSize.width();
    int imgHeight = imgSize.height();

    bool isWidthLarger = false;
    if(imgWidth > imgHeight)
    {
        isWidthLarger = true;
    }

//    QPixmap tempPixmap = QPixmap::fromImage(aImage);
    QImage tempPixmap(aImage) ;
    QImage pm;
    QImage reslutPixMap;
    int x = 0;
    int y = 0;

    if(aIndex >= mRequestQueue.count())
    {
        return;
    }

    ImageRequestToken* tempToken = mRequestQueue[aIndex];
    QString localStr = tempToken->mLocalpath;


    switch (aType)
    {
    case EIcon:
        {
            //qDebug() << "ImageMgr::clipPic  EIcon";
            if(isWidthLarger)
            {
               pm = tempPixmap.scaledToHeight(KIconWidth);
               x = pm.size().width()/2 - KIconWidth/2;
               y = 0;
            }
            else
            {
               pm = tempPixmap.scaledToWidth(KIconWidth);
               x = 0;
               y = pm.size().height()/2 - KIconWidth/2;
            }

            reslutPixMap = pm.copy(x,y,KIconWidth,KIconWidth);
            //qDebug() << " reslutPixMap.save(localStr); before";
            reslutPixMap.save(localStr);
            //qDebug() << " emit tempToken->finished(tempToken->mLocalpath);; before";
            emit tempToken->finished(tempToken->mLocalpath);
            //qDebug() << " emit tempToken->finished(tempToken->mLocalpath); after";
            break;
        }

    case E150150:
        {
            //qDebug() << "ImageMgr::clipPic  E150150";
            if(isWidthLarger)
            {
               pm = tempPixmap.scaledToHeight(K150150Width);
               x = pm.size().width()/2 - K150150Width/2;
               y = 0;
            }
            else
            {
               pm = tempPixmap.scaledToWidth(K150150Width);
               x = 0;
               y = pm.size().height()/2 - K150150Width/2;
            }

            reslutPixMap = pm.copy(x,y,K150150Width,K150150Width);
            reslutPixMap.save(localStr);
            emit tempToken->finished(tempToken->mLocalpath);
            break;
        }
    case E320150:
        {
            //qDebug() << "ImageMgr::clipPic  E320150";
            if(isWidthLarger)
            {
               pm = tempPixmap.scaledToHeight(K320150Height);
               x = pm.size().width()/2 - K320150Width/2;
               y = 0;
            }
            else
            {
               pm = tempPixmap.scaledToWidth(K320150Width);
               x = 0;
               y = pm.size().height()/2 - K320150Height/2;
            }

            reslutPixMap = pm.copy(x,y,K320150Width,K320150Height);
            reslutPixMap.save(localStr);
            emit tempToken->finished(tempToken->mLocalpath);

            break;
        }
    default:
        break;

    }

//    mRequestQueue.removeAt(aIndex);
}

void ImageMgr::addRequest(int aIndex, const QString& aUrl)
{
    LOG_METHOD;

    mDownloadSession->addRequest(aIndex,  aUrl);
    startDownLogo();
}

void ImageMgr::logoDownloadError(int aErrorCode)
{
    Q_UNUSED(aErrorCode)
}

void ImageMgr::startDownLogo()
{
    LOG_METHOD;


    if(NULL != mDownloadSession)
    {
        //qDebug() << "ImageMgr::starDownLogo NULL != mDownloadSession";
        mDownloadSession->startSchedule();
    }
}

void ImageMgr::cancelAllRequest()
{
//    //qDebug() << ImageMgr::cancelAllRequest();
    mDownloadSession->cancelAllRequests();
    clear();
    emit cancel();
}

ImageRequestToken* ImageMgr::addImageRequest( QString url, ImageType aType )
{

    LOG_METHOD;

    ImageRequestToken* token = new ImageRequestToken();

    token->mUrl = url;

    QString postFix = "";

    switch (aType)
    {
    case EIcon :
        postFix = KPostfixIcon;
        break;
    case E150150 :
        postFix = KPostfixClip150150;
        break;
    case E320150 :
        postFix = KPostfixClip320150;
        break;
    default:
        break;
    }

    token->mLocalpath = combianString(url, postFix);
    token->mDone = false;
    token->mType = aType;

    addRequest(mRequestQueue.count(), token->mUrl);
    mRequestQueue.append( token );

    return token;
}

void ImageMgr::cancelImageRequest( ImageRequestToken * token )
{
    Q_UNUSED(token);
}

void ImageMgr::clear()
{
    LOG_METHOD;

    if(!mRequestQueue.isEmpty())
    {
        qDeleteAll(mRequestQueue.begin(), mRequestQueue.end());
        mRequestQueue.clear();
    }
}

void ImageRequestToken::done()
{
    LOG_METHOD;
    emit finished(mLocalpath);
}

void ImageMgr::initFolders()
{
    QString appPath = qApp->applicationDirPath();
    QString imgPath = appPath + "/image";

    QDir imgDir(imgPath);
    if(!imgDir.exists())
    {
        imgDir.mkdir(imgPath);
    }

    mImgPath = imgPath;
}
