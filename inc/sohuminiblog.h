#ifndef _SOHU_MINI_BLOG
#define _SOHU_MINI_BLOG

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QTimer;

//gloable variable for access maneger
extern QNetworkAccessManager* gUniqueNetwrkManager;

class SohuMiniBlog : public QObject
{
    Q_OBJECT

public:
    explicit SohuMiniBlog(QObject * aParent = 0);
    ~SohuMiniBlog();

//interface
public:
    /**
     * set http request header.
     * @aValue, a base64 encoding of data
     */
    Q_INVOKABLE void setHeader(const QString& akey, const QString& aValue);

    /**
     * http get method
     * @aUrl, request url
     */
    Q_INVOKABLE void get(const QString& aUrl);

    /**
     * http delete method
     * @aUrl, request url
     */
    Q_INVOKABLE void deleteResource(const QString& aUrl);

    /**
     * http post method
     * @aUrl, request url
     * @aBody has a default value " "
     */
    Q_INVOKABLE void post(const QString& aUrl, const QString& aBody = QString(" "));

    /**
     * cancel request
     */
    Q_INVOKABLE void cancel();
	
	    /**
     * http upload method
     * @aUrl, request url
     * @aFileName pic file to upload
     */
    Q_INVOKABLE void upload(const QString& aUrl, const QString& aFileName);

    /**
     * post method with pair items
     */
    Q_INVOKABLE void postPairs(const QString& aUrl, const QString& aKey, const QString& aValue);
    Q_INVOKABLE void postPairs(const QString& aUrl, const QString& aKey1, const QString& aValue1,
                                 const QString& aKey2, const QString& aValue2);


signals:
    void loadingFinished(QString aReturnStr);
    void loadingError(QString aErrorStr);

private slots:
    void onReplyFinished();
    void onRequestTimeout();

private:
    QNetworkReply *mReply;
    QNetworkRequest* mRequest;
    QTimer *mRequestTimeoutTimer;
};

#endif /* _SOHU_MINI_BLOG */
