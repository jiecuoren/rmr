#ifndef AUTHENTICATIONCOMMONSTRU_H
#define AUTHENTICATIONCOMMONSTRU_H
#include <QString>

#if 0
typedef struct authParam{
    QString https;
    QString client_id;
    QString redirect_uri;
    QString scope;
    QString response_type;
    QString state;
    QString client_secret;
    QString grant_type;
    QString resrv1;
}stAuthParam;
#endif

typedef struct loginParam{
    QString typeName;
    QString typeValue;
}stLoginParam;
#if 0
accountType;
    QString Email;
    QString Passwd;
    QString service;
//    QString postHttp;

}stLoginParam;
#endif

typedef struct authInfo{
    QString access_token;
    QString expires_in;
    QString token_type;
    QString refresh_token;

}stAuthInfo;

#endif // COMMONSTRU_H
