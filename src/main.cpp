#include <QApplication>
#include <QtDeclarative>
#include <QDeclarativeView>
#include <QDeclarativeEngine>
#include <QDesktopWidget>
#include <QDeclarativeContext>
#include <QTextCodec>

#if defined(Q_OS_WIN32)
#include <QtNetwork/QNetworkProxyFactory>
#endif

#ifdef Q_OS_SYMBIAN
#include <aknappui.h>
//#include <mw/eikenv.h>
//#include <mw/eikaufty.h>
//#include <mw/eikbtgpc.h>
#endif

//#include "dataprovider.h"
#include "rsschannelfile.h"
#include "sohuminiblog.h"
//#include "imagemgr.h"

#include "mrview.h"
#include "feedobject.h"
#include "mrviewmanager.h"
#include "mropmlconfig.h"
#include "mrfeedlistmodel.h"
#include "mrapplicationcontroller.h"
#include "mrsimplemodel.h"
#include "mrdetailmodel.h"
#include "mrimagelistmodel.h"
#include "authenticationmgr.h"

void registerTypes();

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);

    QDeclarativeView view;

    registerTypes();

#if defined(Q_OS_WIN32)
    QNetworkProxyFactory::setUseSystemConfiguration(true);
#endif

    gUniqueNetwrkManager =  view.engine()->networkAccessManager();

    // For Maemo 5 and Symbian use screen resolution but for desktop use different size
#if defined(Q_WS_MAEMO_5) || defined(Q_OS_SYMBIAN) || defined(Q_WS_HARMATTAN) || defined(Q_WS_SIMULATOR)
    // Get screen dimensions
    QDesktopWidget *desktop = QApplication::desktop();
    const QRect screenRect = desktop->screenGeometry();
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
#else
    // On desktop we use nHD
    QPoint topLeft(100,100);
    QSize size(360, 640);
    QRect screenRect(topLeft, size);
#endif


#ifdef Q_OS_SYMBIAN
    if (iAvkonAppUi)
    {
        iAvkonAppUi->SetOrientationL(CAknAppUi::EAppUiOrientationPortrait);
    }

#endif

//#ifdef Q_OS_SYMBIAN
//    if (iAvkonAppUi)
//    {
//        iAvkonAppUi->SetOrientationL(CAknAppUi::EAppUiOrientationPortrait);

//        // Hide the CBA
//        MEikAppUiFactory *factory = CEikonEnv::Static()->AppUiFactory();
//        factory->CreateResourceIndependentFurnitureL(iAvkonAppUi);
//        CEikButtonGroupContainer *cba = CEikButtonGroupContainer::NewL(CEikButtonGroupContainer::ECba,
//                                                                       CEikButtonGroupContainer::EHorizontal,
//                                                                       iAvkonAppUi, 0);
//        CEikButtonGroupContainer *oldCba = factory->SwapButtonGroup(cba);
//        cba->MakeVisible(EFalse);

//    }

//#endif

    QObject::connect(view.engine(), SIGNAL(quit()), &app, SLOT(quit()));    

    // Set the screen size to QML context
    QDeclarativeContext* context = view.rootContext();
    context->setContextProperty("screenWidth", screenRect.width());
    context->setContextProperty("screenHeight", screenRect.height());

    view.setSource(QUrl("qrc:/main.qml"));

#if defined(Q_WS_MAEMO_5) || defined(Q_OS_SYMBIAN) || defined(Q_WS_HARMATTAN) || defined(Q_WS_SIMULATOR)
    view.showFullScreen();
#else
    view.show();
#endif

    return app.exec();
}

void registerTypes()
{
//    qmlRegisterType<DataProvider>("MReader", 1, 0, "DataProvider");
    qmlRegisterType<SohuMiniBlog>("MReader", 1, 0, "SohuMiniBlog");
    qmlRegisterType<MRSimpleModel>("MReader", 1, 0, "MRSimpleModel");
    qmlRegisterType<MRDetailModel>("MReader", 1, 0, "MRDetailModel");
    qmlRegisterType<MrOpmlConfig>("MReader", 1, 0, "MrOpmlConfig");
    qmlRegisterType<MrView>("MReader", 1, 0, "MrView");
    qmlRegisterType<MRApplicationController>("MReader", 1, 0, "MRApplication");
    qmlRegisterType<MrViewManager>("MReader", 1, 0, "MrViewManagment");
    qmlRegisterType<AuthenticationMgr>("MReader",1, 0, "AuthenticationMgr");
    qmlRegisterType<DetailItem>();
    qmlRegisterType<FeedObject>();
    qmlRegisterType<MRFeedListModel>();
    qmlRegisterType<MrImageListModel>();
    qmlRegisterType<MRApplicationControllerAttached>();
}
