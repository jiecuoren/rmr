#ifndef _SMB_HTTP_REQUEST
#define _SMB_HTTP_REQUEST

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QTimer;

//gloable variable for access maneger
extern QNetworkAccessManager* gUniqueNetwrkManager;

class DataProvider : public QObject
{
    Q_OBJECT

public:
    explicit DataProvider(QObject * aParent = 0);
    ~DataProvider();

//interface
public:
    /**
     * set http request header.
     * @aValue, a base64 encoding of data
     */
    Q_INVOKABLE void setHeader(const QString& akey, const QString& aValue);

    /**
     * http get method
     * @aUrl, request url; aType, if you request rss, when finished, we should use different
     *     way to deal with return data.
     */
    Q_INVOKABLE void get(const QString& aUrl);

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

signals:
    void loadingFinished(QByteArray aReturnStr);
    void loadingError(QString aErrorStr);

private slots:
    void onReplyFinished();
    void onRequestTimeout();

private:
    QNetworkReply *mReply;
    QNetworkRequest* mRequest;
    QTimer *mRequestTimeoutTimer;
    QString mUrl;
    bool mRequestAborted;
};

#endif /* _SMB_HTTP_REQUEST */
