#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QDateTime>

#include "sohuminiblog.h"

const int KRequestTimeOut = 60 * 1000; // 60 s
const QString KNetworkError = "Network Error!";
const QString KUserInfoError = "Authorization Failed!";

QNetworkAccessManager* gUniqueNetwrkManager;

SohuMiniBlog::SohuMiniBlog(QObject *aParent) : QObject(aParent), mRequest(NULL)
{
    mRequestTimeoutTimer = new QTimer(this);
    mRequestTimeoutTimer->setSingleShot(true);
    mRequestTimeoutTimer->setInterval(KRequestTimeOut);
    connect(mRequestTimeoutTimer, SIGNAL(timeout()), this, SLOT(onRequestTimeout()));

    mRequest = new QNetworkRequest();
}

SohuMiniBlog::~SohuMiniBlog()
{
    if (mRequestTimeoutTimer->isActive())
    {
        mRequestTimeoutTimer->stop();
    }

    delete mRequest;
    mRequest = NULL;

}

void SohuMiniBlog::setHeader(const QString& akey, const QString& aValue)
{
    qDebug() << "SohuMiniBlog::setHeader"<<akey<< aValue;

    if(akey.isEmpty() || aValue.isEmpty())
    {
        return;
    }

    mRequest->setRawHeader(akey.toAscii(), aValue.toAscii());
}

void SohuMiniBlog::get(const QString& aUrl)
{
    qDebug() << "SohuMiniBlog::get(%S)"<< aUrl;

    if(aUrl.isEmpty())
    {
        return;
    }

    mRequest->setUrl(QUrl(aUrl));

    if(NULL != gUniqueNetwrkManager)
    {
        mRequestTimeoutTimer->start();
        mReply = gUniqueNetwrkManager->get(*mRequest);
        connect(mReply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    }
}

void SohuMiniBlog::deleteResource( const QString& aUrl )
{
    qDebug() << "SohuMiniBlog::delete"<< aUrl;

    if(aUrl.isEmpty())
    {
        return;
    }

    mRequest->setUrl(QUrl(aUrl));

    if(NULL != gUniqueNetwrkManager)
    {
        mRequestTimeoutTimer->start();
        mReply = gUniqueNetwrkManager->deleteResource( *mRequest );
        connect(mReply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    }
}

void SohuMiniBlog::post(const QString& aUrl, const QString& aBody)
{
    qDebug() << "SohuMiniBlog::post(const QString& aUrl, const QString& aBody)";
    qDebug() << aUrl << "  body is   " + aBody.toAscii();

    if(aUrl.isEmpty())
    {
        return;
    }

    mRequest->setUrl(QUrl(aUrl));

    if(NULL != gUniqueNetwrkManager)
    {
        mRequestTimeoutTimer->start();
        mReply = gUniqueNetwrkManager->post(*mRequest, aBody.toUtf8());
        connect(mReply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    }
}

void SohuMiniBlog::postPairs(const QString& aUrl, const QString& aKey, const QString& aValue)
 {
    qDebug() << "SohuMiniBlog::postPairs" << aUrl << aKey << aValue;

    if(aUrl.isEmpty())
    {
        return;
    }

    mRequest->setUrl(QUrl(aUrl));

    QByteArray data;
    QUrl params;

    params.addQueryItem(aKey, aValue);
    data = params.encodedQuery();

    if(NULL != gUniqueNetwrkManager)
    {
        mRequestTimeoutTimer->setInterval(KRequestTimeOut);
        mRequestTimeoutTimer->start();
        mReply = gUniqueNetwrkManager->post(*mRequest, data);
        connect(mReply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    }
 }

void SohuMiniBlog::postPairs(const QString& aUrl, const QString& aKey1, const QString& aValue1,
                                 const QString& aKey2, const QString& aValue2)
{
    qDebug() << "SohuMiniBlog:postPairs" << aUrl << aKey1 << aValue1 << aKey2 << aValue2;
    qDebug() << aUrl;

    if(aUrl.isEmpty())
    {
        return;
    }

    mRequest->setUrl(QUrl(aUrl));

    QByteArray data;
    QUrl params;

    params.addQueryItem(aKey1, aValue1);//addQueryItem
    params.addQueryItem(aKey2, aValue2);//addQueryItem
    data = params.encodedQuery();

    if(NULL != gUniqueNetwrkManager)
    {
        mRequestTimeoutTimer->setInterval(KRequestTimeOut);
        mRequestTimeoutTimer->start();
        mReply = gUniqueNetwrkManager->post(*mRequest, data);
        connect(mReply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    }
}

void SohuMiniBlog::upload(const QString& aUrl, const QString& aFilePath)
{
    qDebug() << "SohuMiniBlog::upload(const QString& aUrl, const QString& aFileName)";
    qDebug() << aUrl << "  body is   " + aFilePath.toUtf8();
    if(aUrl.isEmpty())
    {
        return;
    }

    mRequest->setUrl(QUrl(aUrl));

    QString fileName = aFilePath.section('\\', -1).section('.', 0, 0);
    qsrand(QDateTime::currentDateTime().toTime_t());
    QString b = QVariant(qrand()).toString()+QVariant(qrand()).toString()+QVariant(qrand()).toString();
    QString boundary="---------------------------" + b;

    QByteArray datas(QString("--" + boundary + "\r\n").toAscii());
    datas += "Content-Disposition: form-data; name=\"pic\"\r\n";
    datas += "filename=\"0" + fileName.toUtf8() + ".jpeg\" Content-Type: image/jpeg\r\n\r\n";
    QFile file(aFilePath);
    if ( !file.open(QIODevice::ReadOnly) )
    {
        qDebug() << "SohuMiniBlog::upload open file failed";
        return;
    }

    datas += file.readAll();
    datas += "\r\n";
    datas += QString("--" + boundary + "--\r\n").toAscii();
    qDebug() << QString::number(datas.length());

    mRequest->setHeader(QNetworkRequest::ContentLengthHeader, QString::number(datas.length()));
    mRequest->setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + boundary);

    if(NULL != gUniqueNetwrkManager)
    {
        mRequestTimeoutTimer->setInterval(KRequestTimeOut);
        mRequestTimeoutTimer->start();
        mReply = gUniqueNetwrkManager->post(*mRequest, datas);
        qDebug() << "Start upload:"<<QDateTime::currentDateTime().toString();
        connect(mReply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    }
}

void SohuMiniBlog::cancel()
{
    qDebug() << "SohuMiniBlog::cancel()";

    if(NULL != mReply)
    {
        //can't call "reply->abort()" directly, consider finished request.
        mReply->isFinished() ? mReply->close():mReply->abort();
        mReply->deleteLater();
        mReply = NULL;
    }
}

void SohuMiniBlog::onReplyFinished()
{
     qDebug() << "SohuMiniBlog::onReplyFinished()";
     qDebug() << mReply->error();
     qDebug() << "Sever respond:"<<QDateTime::currentDateTime().toString();

    if(mRequestTimeoutTimer->isActive())
    {
        mRequestTimeoutTimer->stop();
    }

    disconnect(mReply, SIGNAL(finished()), this, SLOT(onReplyFinished()));

    QByteArray tempData = mReply->readAll();
    QNetworkReply::NetworkError replyError = mReply->error();

    mReply->deleteLater();
    mReply = NULL;

    if(QNetworkReply::NoError == replyError)
    {
        emit loadingFinished(tempData);
    }
    else if(QNetworkReply::AuthenticationRequiredError == replyError )
    {
        emit loadingError(KUserInfoError);
    }
    else
    {
        emit loadingError(KNetworkError);
    }

}

void SohuMiniBlog::onRequestTimeout()
{
    qDebug() << "SohuMiniBlog::onRequestTimeout()";

    if((NULL != mReply) && (mReply->isOpen()))
    {
        mReply->close(); // this will in turn emit QNetworkReply::error(QNetworkReply::OperationCanceledError)
    }
    // No need to send loadingError.
}
