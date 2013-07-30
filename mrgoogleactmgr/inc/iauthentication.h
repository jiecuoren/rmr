#ifndef IAUTHENTICATION_H
#define IAUTHENTICATION_H
#include <QString>

#include "authenticationcommonstru.h"
/*
class IAuthenticationRsp
{
    virtual void authenRsp(const authRsp&) = 0;
};
*/

class IAuthentication
{
public:
    virtual bool doLogin(const QString &useName, const QString &passwd = QString()) = 0;
    //virtual bool doLogin() = 0; //reserverd
    //virtual void authenticationRspReg(const IAuthenticationRsp&) = 0;
    //void setAuthParam(const stAuthParam& authEnv) = 0;
    virtual void addLoginParam(const stLoginParam&) = 0;
    virtual void setLoginHttp(const QString&) = 0;

signals:
    virtual void rsp(int msg) = 0;
};


//here the declare is masked;
//Q_DECLARE_INTERFACE(IAuthentication,"com.nokia.IAuthentication/1.0")

#endif // IGOOGLEREADER_H
