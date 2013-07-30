#ifndef IRTHREAD_H
#define IRTHREAD_H

#include <QThread>

class QNetworkAccessManager;

class MrThread : public QThread
{
    Q_OBJECT
public:
    explicit MrThread(QObject *parent = 0);

    QNetworkAccessManager* nwkAccessMgr();

signals:
    void nwkAccessMgrCreated();

protected:
    void run();

private:
    QNetworkAccessManager *mNwkAccessMgr;

};

#endif // IRTHREAD_H
