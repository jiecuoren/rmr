
// INCLUDES
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include "mrdebug.h"

// Consts
#if defined(Q_OS_SYMBIAN)
const QString KLogFileNamePrefix  =  "E:/MReader";
#elif defined(Q_OS_WIN32)
const QString KLogFileNamePrefix  = "MReader";
#endif
const QString KLogFileNameExt = ".txt";
const char * KLogEnter = "====> %s";
const char * KLogExit  = "<==== %s";

// IMPLEMENTATIONS

// ============================================================================
// MethodLogger Defenition
// ============================================================================
MethodLogger::MethodLogger(const char *function) :
    mFunction( function )
{
    QString msg;
    msg.sprintf(KLogEnter, mFunction);
    WRITELOG(msg);
}

MethodLogger::~MethodLogger()
{
    QString msg;
    msg.sprintf(KLogExit, mFunction);
    WRITELOG(msg);
}


// ============================================================================
// FileLogger Defenition
// ============================================================================
bool FileLogger::mFilterByLogMarker = false;
QFile FileLogger::mDebugFile;

void FileLogger::uninstallMessageHandler()
{
    if (mDebugFile.isOpen())
    {
        qInstallMsgHandler(0);
        mDebugFile.close();
    }
}

void FileLogger::installMessageHandler(bool filterByMarker)
{
    QDateTime dateTime(QDateTime::currentDateTime());
    QString logFileName = KLogFileNamePrefix
                        + dateTime.toString("-yyyy-MM-dd-hh-mm-ss-zzz")
                        + KLogFileNameExt;

    mDebugFile.setFileName(logFileName);
    mFilterByLogMarker = filterByMarker;

    if (mDebugFile.open(QIODevice::Text | QIODevice::WriteOnly))
    {
        qInstallMsgHandler(FileLogger::handleMessage);
    }
}

void FileLogger::handleMessage(QtMsgType type, const char *msg)
{
    if (type == QtDebugMsg)
    {
        QString message(msg);
        if (mFilterByLogMarker)
        {
            if(message.contains(KLogMarker))
            {
                QTextStream debugStream(&mDebugFile);
                QDateTime dateTime(QDateTime::currentDateTime());
                QString log2output = dateTime.toString("yyyy-MM-dd::hh:mm:ss.zzz") + " : " + message;
                debugStream<<log2output;
            }
            else
            {
                return;
            }
        }
        else
        {
            QTextStream debugStream(&mDebugFile);
            QDateTime dateTime(QDateTime::currentDateTime());
            QString log2output = dateTime.toString("yyyy-MM-dd::hh:mm:ss.zzz") + " : " + message;
            debugStream<<log2output<<endl;
        }
    }
}

// End of File
