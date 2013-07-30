
#ifndef MRVIEW_H
#define MRVIEW_H

// INCLUDES
#include <QDeclarativeItem>

// CLASS DECLARATIONS
class MrView : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QString viewId READ viewId WRITE setViewId NOTIFY viewIdChanged)
public:
    explicit MrView(QDeclarativeItem *parent = 0);
    ~MrView();

public:
    QString viewId() const;
    void setViewId(const QString &id);

signals:
    void activated(const QString &message);
    void deActivated(const QString &message);
    void viewIdChanged();

private:
    friend class MrViewManager;

    QString mViewId;
};

#endif // MRVIEW_H

// End of File
