
// INCLUDES
#include <QTimer>
#include "mrview.h"
#include "mrviewmanager.h"
#include "mrdebug.h"

// IMPLEMENTATIONS
MrViewManager::MrViewManager(QDeclarativeItem *parent)
    : QDeclarativeItem(parent),
    mCurrentView(NULL),
    mPreviousView(NULL)
{
    LOG_METHOD;
}

MrViewManager::~MrViewManager()
{
    LOG_METHOD;

    foreach (MrView *view, mViews)
    {
        delete view;
        view = NULL;
    }
    mViews.clear();
}

QString MrViewManager::viewSwitchAnimation() const
{
    LOG_METHOD;

    return mViewSwitchAnimation;
}

void MrViewManager::setViewSwitchAnimation(QString viewSwitchAnimation)
{
    LOG_METHOD;

    if (mViewSwitchAnimation != viewSwitchAnimation)
    {
        mViewSwitchAnimation = viewSwitchAnimation;
        emit viewSwitchAnimationChanged();
    }
}

QString MrViewManager::currentViewId() const
{
    LOG_METHOD;

    return mCurrentViewId;
}

void MrViewManager::setCurrentViewId(QString currentViewId)
{
    LOG_METHOD;

    if (!mViews.contains(currentViewId))
        return;

    if (mCurrentViewId != currentViewId)
    {
        mPreviousViewId = mCurrentViewId;
        mCurrentViewId = currentViewId;        
        mPreviousView = mViews.value(mPreviousViewId);
        mCurrentView = mViews.value(mCurrentViewId);

        if (mCurrentView)
        {
            emit mCurrentView->activated(mMessage);
        }
        if (mPreviousView)
        {
            emit mPreviousView->deActivated(mMessage);
        }
        mMessage.clear();

        onActivateView();
        emit currentViewIdChanged();
    }
}

QString MrViewManager::previousViewId() const
{
    LOG_METHOD;

    return mPreviousViewId;
}

MrView *MrViewManager::currentView() const
{
    LOG_METHOD;

    return mCurrentView;
}

MrView *MrViewManager::previousView() const
{
    LOG_METHOD;

    return mPreviousView;
}

QDeclarativeListProperty<MrView> MrViewManager::views()
{
    LOG_METHOD;

    return QDeclarativeListProperty<MrView>(this, 0, &MrViewManager::appendView);
}

void MrViewManager::appendView(QDeclarativeListProperty<MrView> *list, MrView *view)
{
    LOG_METHOD;

    MrViewManager *viewManager = qobject_cast<MrViewManager *>(list->object);
    if (viewManager)
    {
        view->setParentItem(viewManager);
        viewManager->mViews.insert(view->viewId(), view);
        view->setVisible(false);
        connect(view, SIGNAL(viewIdChanged()), viewManager, SLOT(onViewIdChanged()));
    }
}

bool MrViewManager::addView(MrView *view)
{
    LOG_METHOD;

    if (!view)
        return false;

    view->setParentItem(this);
    mViews.insert(view->viewId(), view);
    view->setVisible(false);
    connect(view, SIGNAL(viewIdChanged()), this, SLOT(onViewIdChanged()));

    return true;
}

void MrViewManager::removeView(const QString &viewId)
{
    LOG_METHOD;

    if (mViews.contains(viewId))
    {
        mViews.remove(viewId);
    }
}

MrView *MrViewManager::getView(const QString &viewId)
{
    LOG_METHOD;

    MrView *view = NULL;

    if (mViews.contains(viewId))
    {
        view = mViews.value(viewId);
    }

    return view;
}

void MrViewManager::activateView(const QString &viewId, const QString &message)
{
    LOG_METHOD;

    mMessage = message;
    setCurrentViewId(viewId);
}

int MrViewManager::viewCount() const
{
    LOG_METHOD;

    return mViews.count();
}

void MrViewManager::onActivateView()
{
    LOG_METHOD;

    if (!mCurrentView)
        return;

    mCurrentView->setVisible(true);

    QVariant returnedValue;
    QVariant varCurrentViewId(mCurrentViewId);
    QVariant varPreviousViewId(mPreviousViewId);
    QVariant varAnimationType(mViewSwitchAnimation);
    bool success = QMetaObject::invokeMethod(this, "startAnimation",
                                             Q_RETURN_ARG(QVariant, returnedValue),
                                             Q_ARG(QVariant, varCurrentViewId),
                                             Q_ARG(QVariant, varPreviousViewId),
                                             Q_ARG(QVariant, varAnimationType));
    LOG_FORMAT("MrViewManager::onActivateView success = %d", success);

    if (!success || !returnedValue.toBool())
    {
        animationDone();
    }
}

void MrViewManager::animationAboutToStart()
{
    LOG_METHOD;
}

void MrViewManager::animationDone()
{
    LOG_METHOD;

    mCurrentView->setPos(0.0, 0.0);
    mCurrentView->setZValue(0.0);
    mCurrentView->show();

    if (mPreviousView)
    {
        mPreviousView->setPos(0.0, mPreviousView->pos().y());
        mPreviousView->setZValue(0.0);
        mPreviousView->hide();
    }
}

void MrViewManager::onViewIdChanged()
{
    LOG_METHOD;

    MrView *view = static_cast<MrView*>(sender());
    if (!view)
        return;

    if (mCurrentView == view)
    {
        mCurrentViewId = view->viewId();
    }
    else if (mPreviousView == view)
    {
        mPreviousViewId = view->viewId();
    }

    mViews.insert(view->viewId(), view);
}

// End of File
