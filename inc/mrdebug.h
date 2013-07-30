
#ifndef WTDEBUG_H
#define WTDEBUG_H

// INCLUDES
#include <QDebug>
#include <QFile>

// CONSTS
const QString KLogMarker = "[MReader:] ";

// MACROS
#ifdef LOG_ENABLED
    #define WRITELOG(msg) qDebug()<<KLogMarker<<msg<<"\n";
    #ifdef LOG_TO_FILE
        #ifdef APP_LOG_ONLY
            #define INSTALL_MESSAGE_HANDLER FileLogger::installMessageHandler(true);
        #else
            #define INSTALL_MESSAGE_HANDLER FileLogger::installMessageHandler(false);
        #endif
        #define UNINSTALL_MESSAGE_HANDLER FileLogger::uninstallMessageHandler();
    #else
        #define INSTALL_MESSAGE_HANDLER
        #define UNINSTALL_MESSAGE_HANDLER
    #endif

#else
    #define WRITELOG(msg)
    #define INSTALL_MESSAGE_HANDLER
    #define UNINSTALL_MESSAGE_HANDLER
#endif


// convert QString to Char*, be used with LOG_FORMAT()
#define STRING2CHAR(qstring) qstring.toAscii().constData()

// Example: LOG( "Hello World" );
#define LOG(string) WRITELOG(string)

// Example:
// LOG_METHOD; or LOG_METHOD_ENTER;
#define LOG_METHOD MethodLogger ___methodLogger(__PRETTY_FUNCTION__)
#define LOG_METHOD_ENTER LOG_FORMAT("<--> %s", __PRETTY_FUNCTION__)

// Examples:
// LOG_FORMAT( "integer %d", 10 );
// LOG_FORMAT( "QString %s", STRING2CHAR(someQString) );
#define LOG_FORMAT(fmt,args...) \
    { \
        QString tmp; \
        WRITELOG( tmp.sprintf(fmt,args) ); \
    }

// ============================================================================
// MethodLogger
// ============================================================================
class MethodLogger
{
public:
    MethodLogger(const char *function);
    ~MethodLogger();

private:
    const char *mFunction;
};

// ============================================================================
// FileLogger
// ============================================================================
class FileLogger
{
public:
    static void installMessageHandler(bool filterByMarker);
    static void uninstallMessageHandler();

private:
    static void handleMessage(QtMsgType type, const char *msg);

private:
    static QFile mDebugFile;
    static bool  mFilterByLogMarker;
};

#endif // WTDEBUG_H

// End of File
