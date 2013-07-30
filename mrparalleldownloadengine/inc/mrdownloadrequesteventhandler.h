#ifndef MrDownloadRequestEVENTHANDLER_H
#define MrDownloadRequestEVENTHANDLER_H

class MrDownloadRequest;

class MrDownloadRequestEventHandler
{
public:

    virtual void processFinishedReq(MrDownloadRequest *aRequest) = 0;
};

#endif

