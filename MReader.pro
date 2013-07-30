TEMPLATE = app
TARGET = mreader_0xE7ACAE75
QT += core declarative xml script network

CONFIG += mobility
MOBILITY += systeminfo

OBJECTS_DIR = obj
MOC_DIR = moc
RCC_DIR = rcc

include(mrparalleldownloadengine/mrparalleldownloadengine.pri)
include(mrgoogleactmgr/mrgoogleactmgr.pri)

DEFINES += LOG_ENABLED

INCLUDEPATH += inc

SOURCES += src/main.cpp \
#                        src/dataprovider.cpp \
                        src/rsschannelfile.cpp \
                        src/sohuminiblog.cpp \
                        src/imagemgr.cpp \
                       src/mrdebug.cpp \
                       src/mrfeedlistmodel.cpp \
                       src/mropmlconfig.cpp \
                       src/mrapplicationcontroller.cpp \
                       src/mrview.cpp \
                       src/mrviewmanager.cpp \
                       src/mrsimplemodel.cpp \
                       src/mrdetailmodel.cpp \
                        src/rssfiledownloader.cpp \
                       src/mrimagelistmodel.cpp

HEADERS +=  inc/rsschannelfile.h \
#                        inc/dataprovider.h \
                        inc/sohuminiblog.h \
                        inc/commonvalues.h \
                        inc/imagemgr.h \
                       inc/mrdebug.h \
                       inc/mrfeedlistmodel.h \
                       inc/mropmlconfig.h \
                       inc/mrapplicationcontroller.h \
                       inc/feedobject.h \
                       inc/mrview.h \
                        inc/mrviewmanager.h \
                        inc/mrsimplemodel.h \
                        inc/mrdetailmodel.h \
                        inc/rssfiledownloader.h \
                        inc/mrimagelistmodel.h

RESOURCES += qml/mreader.qrc

OTHER_FILES += qml/*.* \
               qml/component/*.* \
               qml/views/*.* \
               qml/models/*.* \
               qml/resources/local/*.* \
               qml/javascript/* \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

symbian: {
    TARGET.UID3 = 0xE7ACAE75
    TARGET.EPOCHEAPSIZE = 0x80000 0x2000000  #32MB
    TARGET.EPOCSTACKSIZE = 0x20000
    TARGET.CAPABILITY = LocalServices \
        NetworkServices \
        ReadUserData \
        WriteUserData \
        UserEnvironment
        #MultimediaDD \
        #ReadDeviceData \
        #WriteDeviceData \
        #SwEvent

    BLD_INF_RULES.prj_exports += "qml/resources/local/opml_sina.xml /epoc32/data/z/private/opml_sina.xml"

    LIBS += -lavkon \
            -lcone \
            -leikcore \
            -leikcoctl

    ICON = ./RssReader.svg
}

