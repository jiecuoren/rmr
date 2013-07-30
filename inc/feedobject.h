#ifndef FEEDOBJECT_H
#define FEEDOBJECT_H

#include <QObject>

class FeedObject: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ titleStr)
    Q_PROPERTY(QString type READ typeStr)
    Q_PROPERTY(QString url READ urlStr)
    Q_PROPERTY(QString iconUrl READ iconUrlStr)
    Q_PROPERTY(QString htmlUrl READ htmlUrlStr)
    Q_PROPERTY(QString description READ descriptionStr)
    Q_PROPERTY(QString subscribed READ subscribedStr)
    Q_PROPERTY(QString urlLocalPath READ urlLocalPathStr)

public:
  explicit FeedObject(const QString &title,
             const QString &type,
             const QString &url,
             const QString &iconurl,
             const QString &htmlurl,
             const QString &description,
             const QString &subscribed,
             const QString &urllocalpath):
        title(title), type(type),
        url(url), iconUrl(iconurl),
        htmlUrl(htmlurl), description(description),
        subscribed(subscribed), urlLocalPath(urllocalpath){}


  QString titleStr()const{return title;}
  QString typeStr()const{return type;}
  QString urlStr()const{return url;}
  QString iconUrlStr()const{return iconUrl;}
  QString htmlUrlStr()const{return htmlUrl;}
  QString descriptionStr()const{return description;}
  QString subscribedStr()const{return subscribed;}
  QString urlLocalPathStr()const{return urlLocalPath;} // valid when type == folder

  void setLocalPathStr(QString& localpath){urlLocalPath = localpath;}


  QString title;
  QString type;
  QString url;
  QString iconUrl;
  QString htmlUrl;
  QString description;
  QString subscribed;
  QString urlLocalPath;
};

#endif // FEEDOBJECT_H
