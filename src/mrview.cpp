
// INCLUDES
#include "mrview.h"
#include "mrdebug.h"

// IMPLEMENTATIONS
MrView::MrView(QDeclarativeItem *parent)
    :QDeclarativeItem(parent)
{
    LOG_METHOD;
}

MrView::~MrView()
{
    LOG_METHOD;
}

QString MrView::viewId() const
{
    LOG_METHOD;

    return mViewId;
}

void MrView::setViewId(const QString &viewId)
{
    LOG_METHOD;

    if (mViewId != viewId)
    {
        mViewId = viewId;
        emit viewIdChanged();
    }
}

// End of File
