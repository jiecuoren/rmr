#ifndef AUTHENTICATIONMGR_H
#define AUTHENTICATIONMGR_H
#include <QObject>
#include <QSslError>
#include <QMap>
#include <QMultiMap>
#include <QStringList>
#include <QDomElement>
#include "authenticationcommonstru.h"
#include "iauthentication.h"

class QNetworkReply;
class QNetworkAccessManager;
class QFile;
class MRApplicationController;

/*
 * this class take the responsible for authentication;
*/
class AuthenticationMgr : public QObject
{
    Q_OBJECT
    Q_ENUMS(EReqTypr)

public:

    enum EReqTypr
    {
        EReqSID = 0,
        EReqToken,
        EReqFullList
    };

    AuthenticationMgr();
    ~AuthenticationMgr();

    Q_INVOKABLE void doLogin(const QString& useName, const QString& passwd, bool isSave);
    Q_INVOKABLE void getToken();
    Q_INVOKABLE void getFullList();

    Q_INVOKABLE void insertMap(const QString& aCatName, const QString& aTitle, const QString& aLink,bool needCheck);
    Q_INVOKABLE void composeOpml();

    Q_INVOKABLE void syncToGoogle();
    Q_INVOKABLE void saveCachedUrl();
    Q_INVOKABLE QStringList readCachedUrl();
    Q_INVOKABLE void clareCacheFile();
    Q_INVOKABLE void syncOpml();

private:
    const QString getLoginString(const QString& aUserName, const QString& aPassword);
    bool saveUserInfo(const QByteArray &aData, bool aUseGZip = false);
    void paserFullList(const QByteArray &aData);
    void doComposeOpml(const QString& aCatName, QList< QPair<QString, QString> > aList, QDomElement aParent);

signals:
    /*here a msg is emmitted for login info*/
    void loadingError(QString aErrorStr);
    void loadingStarted();
    void loadingFinished(QString aRetStr);

private:
    void parserSid(const QByteArray &aData);
    void packagingHeaderParas();
    void saveFullList(const QByteArray &aData);
private slots:
    void onRequestFinished(QNetworkReply *aNwkReply);
    void errHandler(QNetworkReply*,const QList<QSslError>&);

private:
    QNetworkAccessManager *mpNetAccessMgr;
    bool mClientLoginReturnFlag;
    QString mUserName; //temporally here, it will be replaced if there is database;
    QMap<QString, QString> mParaMap;
    QString mToken;
    EReqTypr mReqType;

    QMultiMap<QString, QPair<QString, QString> > mAllDataMap;
    QStringList mCatNameList;
    QStringList mFinishedCatList;
    QDomDocument doc;
    bool mIsSaveUserInfo;
    QStringList mCachedUrlList;
    QStringList mCachedUrlInFile;
    MRApplicationController* mController;
};

#endif // GOOGLEREADERMGR_H
