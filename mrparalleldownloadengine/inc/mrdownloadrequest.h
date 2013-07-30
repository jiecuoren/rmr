#ifndef IRDOWNLOADREQUEST_H
#define IRDOWNLOADREQUEST_H

#include <QObject>

class QNetworkReply;
class MrDownloadRequestEventHandler;

class MrDownloadRequest : public QObject
{
    Q_OBJECT

public:
    MrDownloadRequest(const int &aIndex,
                      const QString &aUrl,
                      MrDownloadRequestEventHandler   *aEventHandler);

    int                     index() const;
    QString                 url()   const;
    void                    setReply(QNetworkReply *aReply);
    QNetworkReply*          reply() const;
private slots:
    void onRequestFinished();

private:
    int                              mIndex;
    QString                          mUrl;
    QNetworkReply                   *mReply;
    MrDownloadRequestEventHandler   *mEventHandler;
    
};
#endif

