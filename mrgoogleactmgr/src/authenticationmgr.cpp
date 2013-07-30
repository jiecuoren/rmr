#include <qDebug>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QApplication>
#include <QFile>
#include <QSslConfiguration>
#include <QDateTime>
#include <QStringList>
#include <QtScript>
#include <QtCore/QTextStream>
#include <QtAlgorithms>
#include <QDomDocument>
#include <QDomProcessingInstruction>
#include <QDataStream>
#include <mrdebug.h>
#include <mrapplicationcontroller.h>
#include <mropmlconfig.h>
#include "authenticationmgr.h"

const QString GOOGLE_AUTH_HTTP = QString("https://accounts.google.com/o/oauth2/auth?");
const QString GOOGLE_CLIENT_LOGIN_HTTP = QString("https://www.google.com/accounts/ClientLogin");

#if defined(Q_OS_SYMBIAN)
const QString cfgfile = "c:\\private\\E7ACAE75\\opml_sina.xml";
#else
const QString cfgfile = "c:\\opml_sina.xml";
#endif

AuthenticationMgr::AuthenticationMgr() : mReqType(EReqSID), mIsSaveUserInfo(false)
{
    mpNetAccessMgr = new QNetworkAccessManager(this);

    connect(mpNetAccessMgr,
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(onRequestFinished(QNetworkReply*)));

    connect(mpNetAccessMgr,
            SIGNAL(sslErrors(QNetworkReply*,const QList<QSslError>&)),
            this,
            SLOT(errHandler(QNetworkReply*,const QList<QSslError>&)) );

}

AuthenticationMgr::~AuthenticationMgr()
{

}

const QString AuthenticationMgr::getLoginString( const QString& aUserName, const QString& aPassword)
{
    LOG_METHOD;
    QString firstStr = "accountType=HOSTED_OR_GOOGLE&Email=";
    QString andStr = "&";
    QString sencondStr = "Passwd=";
    QString thirdStr = "&service=reader&source=834337477970.apps.googleusercontent.com";
    qDebug() << firstStr + aUserName + andStr + sencondStr + aPassword + thirdStr;
    return firstStr + aUserName + andStr + sencondStr + aPassword + thirdStr;
}

void AuthenticationMgr::doLogin(const QString &userName, const QString &passwd , bool isSave)
{
    LOG_METHOD;
    if(userName.isEmpty() || passwd.isEmpty())
    {
        emit loadingError("User info error");
        return;
    }

    mCatNameList.clear();
    mFinishedCatList.clear();
    mAllDataMap.clear();
    mCachedUrlList.clear();
    mCachedUrlInFile.clear();

    mCachedUrlInFile = readCachedUrl();

    mUserName = userName;

    //now only use file for recording use loginin infomation;
    QString qAppPath = QApplication::applicationDirPath();
    QString qXmlFileName = qAppPath + "/" + userName;

    if (QFile::exists(qXmlFileName))
    {
        qDebug()<<"user name exisiting";
        QFile::remove(qXmlFileName);
    }

    QString postStr = getLoginString(userName, passwd);

    mIsSaveUserInfo = isSave;

    QNetworkRequest req;

    QUrl url(GOOGLE_CLIENT_LOGIN_HTTP);
    req.setUrl(url);

    mReqType = EReqSID;

    emit loadingStarted();

    QNetworkReply *reply = mpNetAccessMgr->post(req,QByteArray(postStr.toAscii()));

    //the SSL Errors should be ignored;
    reply->ignoreSslErrors();

}

void AuthenticationMgr::parserSid(const QByteArray &aData)
{
    LOG_METHOD;

    QString returnData(aData);

    QStringList strList = returnData.split("\n");

    QString strItor;
    QStringList tempList;
    foreach(strItor, strList)
    {
        tempList.clear();
        tempList = strItor.split("=", QString::SkipEmptyParts);
        if(!tempList.isEmpty())
        {
            mParaMap.insert(tempList.first(), tempList.last());
        }
    }

}

void AuthenticationMgr::getToken()
{
    LOG_METHOD;
    mToken.clear();

    QString getHttp = "https://www.google.com/reader/api/0/token?client=834337477970.apps.googleusercontent.com&ck=";
    QString oauthTimestamp = QString("%1").arg(QDateTime::currentDateTime().toTime_t());
    getHttp += oauthTimestamp;

    QString head1 = "Authorization";
    QString head1Value = "GoogleLogin auth=" + mParaMap.value("Auth");

    QString head2 = "Cookie";
    QString head2Value = "SID=" + mParaMap.value("SID");
    QNetworkRequest req;

    QUrl url(getHttp);
    req.setUrl(url);
    req.setRawHeader(head1.toAscii(), head1Value.toAscii());
    req.setRawHeader(head2.toAscii(), head2Value.toAscii());

     mReqType = EReqToken;

    QNetworkReply *reply = mpNetAccessMgr->get(req);
    reply->ignoreSslErrors();
}

void AuthenticationMgr::getFullList()
{
    qDebug() << "AuthenticationMgr::requestFullList()";



    QString oauthTimestamp = QString("%1").arg(QDateTime::currentDateTime().toTime_t());
    QString getHttp = "https://www.google.com/reader/api/0/subscription/list?client=834337477970.apps.googleusercontent.com&ck=" + oauthTimestamp +  "&output=json";

    QString head1 = "Authorization";
    QString head1Value = "GoogleLogin auth=" + mParaMap.value("Auth");

    QString head2 = "Cookie";
    QString head2Value = "SID=" + mParaMap.value("SID");

    QString head3 = "accept-encoding";
    QString head3Value = "gzip, deflate";

    QNetworkRequest req;

    QUrl url(getHttp);
    req.setUrl(url);
    req.setRawHeader(head1.toAscii(), head1Value.toAscii());
    req.setRawHeader(head2.toAscii(), head2Value.toAscii());
    req.setRawHeader(head3.toAscii(), head3Value.toAscii());

    mReqType = EReqFullList;
//    mCachedUrlInFile.clear();
//    mCachedUrlInFile = readCachedUrl();
//    qDebug() << "mCachedUrlInFile in getFullList" << mCachedUrlInFile;
    QNetworkReply *reply = mpNetAccessMgr->get(req);
    reply->ignoreSslErrors();
}

void AuthenticationMgr::packagingHeaderParas()
{

}


void AuthenticationMgr::errHandler(QNetworkReply* netRly,const QList<QSslError>& err)
{
    LOG_METHOD;
    emit loadingError("SSL Error");
}

void AuthenticationMgr::onRequestFinished(QNetworkReply *aNetReply)
{
    LOG_METHOD;

    QByteArray responeData = aNetReply->readAll();

    if(aNetReply->error() != QNetworkReply::NoError)
    {
       // aNetReply->ignoreSslErrors();
        qDebug()<<"Error received"<<aNetReply->errorString();
        aNetReply->deleteLater();
        aNetReply = NULL;
        emit loadingError("Request Error!");
    }
    else
    {
        switch (mReqType)
        {
        case EReqSID:
//            if(mIsSaveUserInfo)
//            {
                saveUserInfo(responeData);
//            }
            parserSid(responeData);
            getFullList();
            break;

        case EReqToken:
            mToken = QString(responeData);
            break;

        case EReqFullList:
//            saveFullList(responeData);
            emit loadingFinished(QString(responeData));
            break;

        default:
            break;
        }
    }

    return;
}

void AuthenticationMgr::saveFullList(const QByteArray &aData)
{
    LOG_METHOD;
    QString fullFile = QApplication::applicationDirPath() + "/" + "fullList.xml";
    QFile file(fullFile);

    if(file.exists())
    {
        file.remove();
    }

    file.open(QIODevice::WriteOnly);

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << aData;

    file.close();

}


bool AuthenticationMgr::saveUserInfo(const QByteArray &aData, bool aUseGZip)
{

    LOG_METHOD;

    QString qAppPath = QApplication::applicationDirPath();

    QString qZipFileName = qAppPath + "/" + "ResponseZipFile";
    QString qXmlFileName = qAppPath + "/" + mUserName;

    if (QFile::exists(qZipFileName))
    {
        QFile::remove(qZipFileName);
    }

    if (QFile::exists(qXmlFileName))
    {
        QFile::remove(qXmlFileName);
    }

    QFile gzipFile(qZipFileName);
    QFile xmlFile(qXmlFileName);

    if(aUseGZip)
    {
        if (!gzipFile.open(QIODevice::WriteOnly))
        {
            return false;
        }

        qint64 res = gzipFile.write(aData);

        gzipFile.flush();
        gzipFile.close();
        if(res <= 0)
        {
            return false;
        }

        return true;
    }


    if(!aUseGZip)
    {
        if (!xmlFile.open(QIODevice::WriteOnly))
        {
            return false;
        }

        qint64 ret = xmlFile.write(aData);

        xmlFile.flush();
        xmlFile.close();
        if(ret <= 0)
        {
            QFile::remove(qXmlFileName);
            //res = false;
        }

        return (ret > 0);
    }

    return true;

}

void AuthenticationMgr::insertMap(const QString& aCatName, const QString& aTitle, const QString& aLink, bool needCheck)
{
    LOG_METHOD;

    if(aTitle.isEmpty() || aLink.isEmpty())
    {
        return;
    }

    QString subLink = aLink.right(aLink.length() - 5) ;
    qDebug() << "subLink is " << subLink;

//    qDebug() << "mCachedUrlInFile is in insertMap is " << mCachedUrlInFile;

    if(needCheck)
    {
        if(mCachedUrlInFile.contains(subLink))
        {
            return;
        }
    }

    mCachedUrlList.append(subLink);

    QPair <QString, QString> tempPair;
    tempPair.first= aTitle;
    tempPair.second = subLink;

    // give a space char
    QString tempCatName = " ";

    if(!aCatName.isEmpty())
    {
        tempCatName = aCatName;
    }

    if(!mCatNameList.contains(tempCatName))
    {
        mCatNameList.append(tempCatName);
    }

    mAllDataMap.insert(tempCatName, tempPair);

}

void AuthenticationMgr::composeOpml()
{
    qDebug() << "AuthenticationMgr::composeOpml" ;

    if(mAllDataMap.isEmpty())
    {
        return;
    }

    mFinishedCatList.clear();

//    if(NULL == mController)
//    {
//        mController = MRApplicationController::getController();
//    }

    QString tempFilePath = qApp->applicationDirPath() + "/opml.xml";

    qDebug() << "auth tempFilePath " << tempFilePath;

    QFile opmlFile(tempFilePath);
    if(opmlFile.exists())
    {
        opmlFile.remove();
    }

    if(!opmlFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return ;

    QDomProcessingInstruction instruction;
    instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    QDomProcessingInstruction instruction2;
    instruction2 = doc.createProcessingInstruction("opml","version=\"1.0\"");
    doc.appendChild(instruction);

    QDomElement opml = doc.createElement("opml");
    QDomAttr version = doc.createAttribute("version");
    version.setValue("1.0");
    opml.setAttributeNode(version);

    QDomElement head = doc.createElement("head");

    QDomElement title = doc.createElement("title");
    QDomText sina = doc.createTextNode("all rss");
    title.appendChild(sina);

    QDomElement meta = doc.createElement("meta");
    QDomAttr httpEquiv = doc.createAttribute("http-equiv");
    httpEquiv.setValue("X-UA-Compatible");
    QDomAttr content = doc.createAttribute("content");
    content.setValue("IE=EmulateIE7");
    meta.setAttributeNode(httpEquiv);
    meta.setAttributeNode(content);

    QDomElement body = doc.createElement("body");

    head.appendChild(title);
    head.appendChild(meta);

     doc.appendChild(opml);

     opml.appendChild(head);
     opml.appendChild(body);

     mFinishedCatList.clear();

    for(int index=0; index < mCatNameList.count(); ++index)
    {
        QString tempCatName = mCatNameList.at(index);
        if(!mFinishedCatList.contains(tempCatName))
        {
            QList< QPair<QString, QString> > valueList = mAllDataMap.values(tempCatName);
            doComposeOpml(tempCatName, valueList, body);
            mFinishedCatList.append(tempCatName);
        }
    }

     QTextStream out(&opmlFile);
//     out.setCodec("UTF-8");
     doc.save(out,4, QDomNode::EncodingFromTextStream);

     opmlFile.close();


}

void AuthenticationMgr::doComposeOpml(const QString& aCatName, QList< QPair<QString, QString> > aList,QDomElement aParent)
{
    LOG_METHOD;

//    qDebug() << "aCatName is " << aCatName;
//    qDebug() << "aList is " << aList;

    if(" " != aCatName)
    {
        //folder name line
        QDomElement parent = doc.createElement("outline");
        QDomAttr text = doc.createAttribute("text");
        text.setValue(aCatName);
        QDomAttr title = doc.createAttribute("title");
        title.setValue(aCatName);
        QDomAttr type = doc.createAttribute("type");
        type.setValue("folder");

        parent.setAttributeNode(text);
        parent.setAttributeNode(title);
        parent.setAttributeNode(type);

        // each feed
        for(int index=0; index < aList.count(); ++index)
        {
            QString feedTitle = aList.at(index).first;
            QString feedLink = aList.at(index).second;

//            qDebug() << "feedTitle is " << feedTitle;

            QDomElement feed = doc.createElement("outline");

            QDomAttr text = doc.createAttribute("text");
            text.setValue(feedTitle);
            QDomAttr title = doc.createAttribute("title");
            title.setValue(feedTitle);
            QDomAttr type = doc.createAttribute("type");
            type.setValue("rss");
            QDomAttr xmlUrl = doc.createAttribute("xmlUrl");
            xmlUrl.setValue(feedLink);

            feed.setAttributeNode(text);
            feed.setAttributeNode(title);
            feed.setAttributeNode(type);
            feed.setAttributeNode(xmlUrl);

            parent.appendChild(feed);
        }

        aParent.appendChild(parent);
    }
    else
    {
        int tempIndex = 1;
        QList< QPair<QString, QString> > tempList;

        while(aList.count() > 10)
        {

            tempList.append(aList.takeFirst());

            if(tempList.count() == 10)
            {
                QString tempCatStr = "No name-" + QString::number(tempIndex);
                doComposeOpml(tempCatStr,tempList, aParent);
                tempList.clear();
                tempIndex++;
            }
        }

         //each feed
        for(int index=0; index < aList.count(); ++index)
        {
            QString feedTitle = aList.at(index).first;
            QString feedLink = aList.at(index).second;

//            qDebug() << "feedTitle is " << feedTitle;

            QDomElement feed = doc.createElement("outline");

            QDomAttr text = doc.createAttribute("text");
            text.setValue(feedTitle);
            QDomAttr title = doc.createAttribute("title");
            title.setValue(feedTitle);
            QDomAttr type = doc.createAttribute("type");
            type.setValue("rss");
            QDomAttr xmlUrl = doc.createAttribute("xmlUrl");
            xmlUrl.setValue(feedLink);

            feed.setAttributeNode(text);
            feed.setAttributeNode(title);
            feed.setAttributeNode(type);
            feed.setAttributeNode(xmlUrl);

            aParent.appendChild(feed);
        }
    }
}

// will later
void AuthenticationMgr::syncToGoogle()
{
    LOG_METHOD;

}

void AuthenticationMgr::saveCachedUrl()
{
    qDebug() << "AuthenticationMgr::saveCachedUrl()";
    if(!mCachedUrlList.isEmpty())
    {
        QFile urlFile(QApplication::applicationDirPath() + "/" + "cachedurl.dat");

        if(urlFile.exists())
        {
            urlFile.remove();
        }

        urlFile.open(QIODevice::WriteOnly);
        QDataStream outStream(&urlFile);

        QStringList tempStrList = mCachedUrlInFile + mCachedUrlList;
        outStream << tempStrList ;

        urlFile.close();

        mCachedUrlList.clear();
    }
}

QStringList AuthenticationMgr::readCachedUrl()
{
    qDebug() << "AuthenticationMgr::readCachedUrl()";

    QStringList strList;

    QFile urlFile(QApplication::applicationDirPath() + "/" + "cachedurl.dat");
    if(urlFile.exists())
    {
        urlFile.open(QFile::ReadOnly);
        QDataStream inStream(&urlFile);

        inStream >> strList;
        urlFile.close();
    }
//    qDebug() << "strList is  " << strList;
    return strList;
}

void AuthenticationMgr::clareCacheFile()
{
    qDebug() << "AuthenticationMgr::clareCacheFile()";


    QFile urlFile(QApplication::applicationDirPath() + "/" + "cachedurl.dat");
    if(urlFile.exists())
    {
        urlFile.remove();
    }
}

void AuthenticationMgr::syncOpml()
{
    LOG_METHOD;
    qDebug() << "mAllDataMap" << mAllDataMap;

    if(mAllDataMap.isEmpty())
    {
        return;
    }

    QMultiMap<QString, QPair<QString, QString> >::iterator i = mAllDataMap.begin();
    // we just give a new name
    QString tempCatName = "";
    QString tempLink = "";
    QString tempFeedName = "";

    while (i != mAllDataMap.end())
    {
        tempCatName = i.key();

        if(tempCatName.trimmed().isEmpty())
        {
            qDebug() << "tempCatName.isEmpty() is " << tempCatName;
            tempCatName = "New Feeds";
        }

        tempLink = i.value().second;
        tempFeedName = i.value().first;
        if(NULL == mController)
        {
            mController = MRApplicationController::getController();
        }

        qDebug() << "tempLink,tempFeedName, tempCatName is " << tempLink << "---" << "---" << tempFeedName << "---" << tempCatName;

        MRApplicationController::opmlConfig()->addItemBySync(tempLink,tempFeedName, tempCatName);

        ++i;
    }

}
