#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QApplication>
#include <QUrl>
#include <QTimer>
#include <QRegExp>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include "mrdebug.h"

#include "dataprovider.h"
#include "commonvalues.h"

QNetworkAccessManager* gUniqueNetwrkManager;

DataProvider::DataProvider(QObject *aParent) : QObject(aParent),
    mRequest(NULL),mUrl(""), mRequestAborted(false)
{
    mRequestTimeoutTimer = new QTimer(this);
    mRequestTimeoutTimer->setSingleShot(true);
    mRequestTimeoutTimer->setInterval(KRequestTimeOut);
    connect(mRequestTimeoutTimer, SIGNAL(timeout()), this, SLOT(onRequestTimeout()));

    mRequest = new QNetworkRequest();
}

DataProvider::~DataProvider()
{
    if (mRequestTimeoutTimer->isActive())
    {
        mRequestTimeoutTimer->stop();
    }

    delete mRequest;
    mRequest = NULL;

}

void DataProvider::setHeader(const QString& akey, const QString& aValue)
{
    LOG_METHOD;

    if(akey.isEmpty() || aValue.isEmpty())
    {
        return;
    }

    mRequest->setRawHeader(akey.toAscii(), aValue.toAscii());
}

void DataProvider::get(const QString& aUrl)
{
    LOG_METHOD;
    qDebug() << aUrl;

    if(aUrl.isEmpty())
    {
        qDebug() << "DataProvider::get(const QString& aUrl)  aUrl.isEmpty()";
        return;
    }

    mRequestAborted = false;

    mRequest->setUrl(QUrl(aUrl));

    if(NULL != gUniqueNetwrkManager)
    {
        qDebug() << "NULL != gUniqueNetwrkManager";
        mRequestTimeoutTimer->start();
        mReply = gUniqueNetwrkManager->get(*mRequest);
        connect(mReply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    }
}

void DataProvider::post(const QString& aUrl, const QString& aBody)
{
    LOG_METHOD;
    qDebug() << aUrl << "  body is   " + aBody.toAscii();

    if(aUrl.isEmpty())
    {
        return;
    }

    mRequestAborted = false;

    mRequest->setUrl(QUrl(aUrl));

    if(NULL != gUniqueNetwrkManager)
    {
        mRequestTimeoutTimer->start();
        mReply = gUniqueNetwrkManager->post(*mRequest, aBody.toUtf8());
        connect(mReply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    }
}

void DataProvider::cancel()
{
    LOG_METHOD;
    mRequestAborted = true;
    mReply->abort();
}

void DataProvider::onReplyFinished()
{
     LOG_METHOD;

    if(mRequestTimeoutTimer->isActive())
    {
        mRequestTimeoutTimer->stop();
    }

    if(NULL == mReply)
    {
        return;
    }

    if(mRequestAborted)
    {
        mReply->deleteLater();
        mReply = NULL;
        return;
    }

    QByteArray tempData = mReply->readAll();
    QNetworkReply::NetworkError errorCode = mReply->error();

    disconnect(mReply, SIGNAL(finished()), this, SLOT(onReplyFinished()));

    mReply->deleteLater();
    mReply = NULL;

//    qDebug() << "error code is " << errorCode;
//    qDebug() << "tempData is \n" << tempData;

    if(QNetworkReply::NoError == errorCode)
    {
        emit loadingFinished(tempData);
    }
    else if(QNetworkReply::AuthenticationRequiredError == errorCode )
    {
        emit loadingError(KUserInfoError);
    }
    else
    {
        emit loadingError(KNetworkError);
    }

}

void DataProvider::onRequestTimeout()
{
    LOG_METHOD;

    cancel();
    emit loadingError(KNetworkError);
    // No need to send loadingError.
}
