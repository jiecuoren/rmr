
#ifndef MRVIEWMANAGER_H
#define MRVIEWMANAGER_H

// INCLUDES
#include <QDeclarativeListProperty>
#include <QDeclarativeItem>
#include <QMap>

// FORWARD DECLARATIONS
class MrView;

// CLASS DECLARATIONS
class MrViewManager : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QString viewSwitchAnimation READ viewSwitchAnimation WRITE setViewSwitchAnimation NOTIFY viewSwitchAnimationChanged)
    Q_PROPERTY(QString currentViewId READ currentViewId WRITE setCurrentViewId NOTIFY currentViewIdChanged)
    Q_PROPERTY(QString previousViewId READ previousViewId)
    Q_PROPERTY(MrView* currentView READ currentView)
    Q_PROPERTY(MrView* previousView READ previousView)
    Q_PROPERTY(QDeclarativeListProperty<MrView> views READ views)

public:
    explicit MrViewManager(QDeclarativeItem *parent = 0);
    ~MrViewManager();

public:
    QString viewSwitchAnimation() const;
    void setViewSwitchAnimation(QString viewSwitchAnimation);

    QString currentViewId() const;
    void setCurrentViewId(QString currentViewId);
    QString previousViewId() const;

    MrView *currentView() const;
    MrView *previousView() const;

    QDeclarativeListProperty<MrView> views();

signals:
    void viewSwitchAnimationChanged();
    void currentViewIdChanged();

public slots:
    bool addView(MrView *view);
    void removeView(const QString &viewId);
    MrView *getView(const QString &viewId);
    void activateView(const QString &viewId, const QString &message);
    int viewCount() const;
    void animationAboutToStart();
    void animationDone();

private slots:
    void onActivateView();
    void onViewIdChanged();

private:
    static void appendView(QDeclarativeListProperty<MrView> *list, MrView *view);

private:
    QString mViewSwitchAnimation;
    QString mCurrentViewId;
    QString mPreviousViewId;
    MrView *mCurrentView;
    MrView *mPreviousView;
    QMap<QString, MrView *> mViews;

    QString mMessage;
};

#endif // MRVIEWMANAGER_H

// End of File
