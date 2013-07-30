#include <QFile>
#include <QDateTime>
#include <QApplication>
#include <QDir>
#include <mrapplicationcontroller.h>
#include "mrdebug.h"
#include "mropmlconfig.h"
#include "mrfeedlistmodel.h"

const QString fileName = "opml.xml";

const int indentSize = 4;
const QString defaultValue = "";
const QString defaultTypeValue = "folder";
const char* mrErrorStr[] = {
    "no error",
    "file format not support",
    "out of bourn"};

typedef enum {
    MR_ERR_NONE = 0,
    MR_ERR_NOSUPPORT,
    MR_ERR_BOURN
}MrErrorId;

MrOpmlConfig* MrOpmlConfig::mThis = NULL;

MrOpmlConfig::MrOpmlConfig(QObject* parent):QObject(parent), mSerialNum(0), mModified(true)
            ,mUserName(""),mAppFilePath("")
{
    LOG_METHOD;
    mFolderListModel = new MRFeedListModel();
    mAppFilePath = qApp->applicationDirPath();
    cfgfile = mAppFilePath + "/" + fileName;
    cfgfileout = cfgfile;
//    mrLoadXml();
}

void MrOpmlConfig::initUserFolder(QString aUserName)
{
    LOG_METHOD;

    qDebug() << "initUserFolder mUserName is" << aUserName;

    QString cfgfileDirPath = mAppFilePath + "\\" + aUserName.remove(".").remove("@");

    qDebug() << "cfgfileDirPath is " << cfgfileDirPath;

    QDir cfgfileDir(cfgfileDirPath);
    if(!cfgfileDir.exists())
    {
        cfgfileDir.mkdir(cfgfileDirPath);
    }

    mLocalDir = cfgfileDirPath + "/xml/";

    QDir xmlDir(mLocalDir);

    if(!xmlDir.exists())
    {
        xmlDir.mkdir(mLocalDir);
    }

}

MrOpmlConfig::~MrOpmlConfig()
{
    LOG_METHOD;

    mrSaveXml();

    if (mFolderListModel != NULL)
    {
        qDebug() << "~MrOpmlConfig(), mFolderListModel != NULL" ;
        delete mFolderListModel;
        mFolderListModel = NULL;
    }

    clearFeedList();

}

// public application interface

MrOpmlConfig* MrOpmlConfig::openInstance()
{
    LOG_METHOD;

    if ( NULL == mThis )
        mThis = new MrOpmlConfig();

    return mThis;

}

void MrOpmlConfig::closeInstance()
{
    LOG_METHOD;
    if (this == mThis)
        delete mThis;
    mThis = NULL;
}

bool MrOpmlConfig::mrLoadXml()
{
    LOG_METHOD;

    QString errorStr;
    int errorLine;
    int errorColumn;

    qDebug() << "mrLoadXml cfgfile is" << cfgfile;

    if (cfgfile.isEmpty())
    {
        return false;
    }


    QFile file(cfgfile);
    if (!file.open(QFile::ReadWrite | QFile::Text)) {
        LOG_FORMAT("open fail:%s, %s", cfgfile.toLatin1().data(), file.errorString().toLatin1().data());
        return false;
    }


    if (!mDomDocument.setContent(&file, true, &errorStr, &errorLine,
                           &errorColumn)){
        LOG_FORMAT("dom setcontent fail: %s", errorStr.toLatin1().data());
        return false;
    }
    file.close();

    QDomElement root = mDomDocument.documentElement();

    if (root.tagName() != "opml"){
        emit error(mrErrorStr[MR_ERR_NOSUPPORT]);
    }

    updateConfigPart(true);

    parseModelName(root);

    QDomElement child = root.firstChildElement("body");

    initFolderList(child);

    LOG_FORMAT("opml file loaded: %s", cfgfile.toLatin1().data());

    emit loaded();

    return true;
}

void MrOpmlConfig::mrSaveXml()
{
    LOG_METHOD;

    if (cfgfileout.isEmpty())
        return;

    QFile file(cfgfileout);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }

    updateConfigPart(false);

    QTextStream out(&file);
    mDomDocument.save(out, indentSize);
    return;
}

void MrOpmlConfig::updateConfigPart(bool fromOpml)
{
    LOG_METHOD;
    QDomElement root = mDomDocument.documentElement();

    QDomElement child = root.firstChildElement();
    if (child.tagName() != "config") {
        root.insertBefore(mDomDocument.createElement("config"), root.firstChild());
        child = root.firstChildElement();

        QDomElement tmpNode = mDomDocument.createElement("serialnum");
        tmpNode.appendChild(mDomDocument.createTextNode("0"));
        child.appendChild(tmpNode);
        tmpNode = mDomDocument.createElement("modified");
        tmpNode.appendChild(mDomDocument.createTextNode("true"));
        child.appendChild(tmpNode);
       }

    if (fromOpml){
        mSerialNum = child.firstChildElement("serialnum").text().toUInt();
        mModified = child.firstChildElement("modified").text() != "false";
    }else{
        QDomElement tmpNode = child.firstChildElement("serialnum");
        QString serialNumStr;
        serialNumStr.setNum(mSerialNum);
        tmpNode.firstChild().setNodeValue(serialNumStr);

        tmpNode = child.firstChildElement("modified");
        tmpNode.firstChild().setNodeValue(mModified ? "true":"false");
    }

    if (mModified) {
       adjustLocalPath();
       mModified = false;
    }
}

void MrOpmlConfig::adjustLocalPath()
{
    LOG_METHOD;
    QDomNodeList tempList = mDomDocument.elementsByTagName("outline");
    int index = 0;

    while(index < tempList.count())
    {
        QDomElement tempNode = tempList.at(index).toElement();
        if ("rss" == tempNode.attribute("type") &&
                "" == tempNode.attribute("urllocalpath")) {

            QString localPath = QString();
            localPath = mLocalDir + localPath.setNum(getNextSerialNum(),16) + ".xml";
            tempNode.setAttribute("urllocalpath", localPath);
        }
        index++;
    }

}

MRFeedListModel* MrOpmlConfig::getFeedListByFolderName(const QString& folder)
{
    LOG_METHOD;

    MRFeedListModel* result = NULL;

    if ((result = feedList(folder)) == NULL){

        QDomElement child = getFolderElement(folder);

        if (child.isNull())
            return NULL;

        LOG_FORMAT("child tag name:%s", child.tagName().toLatin1().data())


        result = new MRFeedListModel(folder, this);

        child = child.firstChildElement("outline");
        while (!child.isNull()){
            LOG_FORMAT("child loop tag name:%s", child.attribute("xmlUrl").toLatin1().data())
            addFeedItem(result, child);
            child = child.nextSiblingElement("outline");
        }
        mFeedModelList.append(result);
    }

    return result;

}

bool MrOpmlConfig::addItemByUser(const QString& aFeedUrl, const QString& aFeedName,
                           const QString& aPageName)
{
    LOG_METHOD;

    if(aFeedUrl.isEmpty() || aFeedName.isEmpty() || (aPageName.isEmpty()) )
    {
        return false;
    }

    qDebug() << aFeedUrl << "--" << aFeedName << "---" << aPageName;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (cfgfile.isEmpty())
        return false;

    QFile file(cfgfile);
    if (!file.open(QFile::ReadWrite | QFile::Text)) {
        LOG_FORMAT("open fail:%s, %s", cfgfile.toLatin1().data(), file.errorString().toLatin1().data());
        return false;
    }

    if (!mDomDocument.setContent(&file, true, &errorStr, &errorLine,
                           &errorColumn)){
        LOG_FORMAT("dom setcontent fail: %s", errorStr.toLatin1().data());
        return false;
    }

    file.close();

    QDomNodeList tempList = mDomDocument.elementsByTagName("outline");
    int index = 0;
    QDomNode tempNode;
    // find node by give pagename
    while(index < tempList.count())
    {
        tempNode = tempList.at(index);
        if(aPageName == tempNode.toElement().attribute("title"))
        {
            break;
        }
        ++index;
        qDebug() << "index is  " << index;
    }

    // make a new tempElement, and set attributes
    QDomElement tempElement = mDomDocument.createElement("outline");
    tempElement.setAttribute("text","text1");
    tempElement.setAttribute("title",aFeedName);
    tempElement.setAttribute("type","rss");
    tempElement.setAttribute("xmlUrl",aFeedUrl);
    tempElement.setAttribute("htmlUrl","www.sina.com.cn");

    tempNode.appendChild(tempElement);

    //save to file. delete old file
    if(file.exists())
    {
        file.remove();
    }

    if (!file.open(QFile::ReadWrite | QFile::Text)) {
        LOG_FORMAT("open fail:%s, %s", cfgfile.toLatin1().data(), file.errorString().toLatin1().data());
        return false;
    }

    QTextStream out(&file);
    mDomDocument.save(out, indentSize);
    file.close();

    // update model.
    MRFeedListModel* tempModel = getFeedListByFolderName(aPageName);
    addFeedItem(tempModel, tempElement);
    tempModel->refresh();
    return true;
}

bool MrOpmlConfig::renameItemByUser(const QString& aOldName, const QString& aNewName, const QString& aPageName, const int aItemIndex)
{
    LOG_METHOD;

    if(aOldName.isEmpty() || aNewName.isEmpty() || (aPageName.isEmpty()) || (aItemIndex < 0))
    {
        return false;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (cfgfile.isEmpty())
        return false;

    QFile file(cfgfile);
    if (!file.open(QFile::ReadWrite | QFile::Text)) {
        LOG_FORMAT("open fail:%s, %s", cfgfile.toLatin1().data(), file.errorString().toLatin1().data());
        return false;
    }

    if (!mDomDocument.setContent(&file, true, &errorStr, &errorLine,
                           &errorColumn)){
        LOG_FORMAT("dom setcontent fail: %s", errorStr.toLatin1().data());
        return false;
    }

    file.close();

    QDomNodeList tempList = mDomDocument.elementsByTagName("outline");
    int index = 0;
    QDomNode tempNode;

    // find node by give pagename
    while(index < tempList.count())
    {
        tempNode = tempList.at(index);
        if(aPageName == tempNode.toElement().attribute("title"))
        {
            break;
        }
        ++index;
        qDebug() << "index is  " << index;
    }


    QDomNodeList tempChildrenList = tempNode.childNodes();

    if(tempChildrenList.isEmpty())
    {
        return false;
    }

    index = 0;
    QDomNode tempChildNode;
    // find child by the give name.
    while(index < tempChildrenList.count())
    {
        tempChildNode = tempChildrenList.at(index);
        if(aOldName == tempChildNode.toElement().attribute("title"))
        {
            break;
        }
        ++index;
        qDebug() << "index is  " << index;
    }


    // set new name
    tempChildNode.toElement().setAttribute("title", aNewName);

    // delete old file
     if(file.exists())
     {
         file.remove();
     }

     if (!file.open(QFile::ReadWrite | QFile::Text)) {
         LOG_FORMAT("open fail:%s, %s", cfgfile.toLatin1().data(), file.errorString().toLatin1().data());
         return false;
     }

    QTextStream out(&file);
    mDomDocument.save(out, indentSize);
    file.close();

    //update model
    MRFeedListModel* tempModel = getFeedListByFolderName(aPageName);
    FeedObject* tempObject = tempModel->data(aItemIndex);
    tempObject->title = aNewName;
    tempModel->refresh();

    return true;
}

bool MrOpmlConfig::deleteItemByUser(const QString& aItemName, const QString& aPageName, const int aItemIndex)
{
    LOG_METHOD;

    if((aPageName.isEmpty()) || (aItemIndex < 0))
    {
        return false;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (cfgfile.isEmpty())
        return false;

    QFile file(cfgfile);
    if (!file.open(QFile::ReadWrite | QFile::Text)) {
        LOG_FORMAT("open fail:%s, %s", cfgfile.toLatin1().data(), file.errorString().toLatin1().data());
        return false;
    }

    if (!mDomDocument.setContent(&file, true, &errorStr, &errorLine,
                           &errorColumn)){
        LOG_FORMAT("dom setcontent fail: %s", errorStr.toLatin1().data());
        return false;
    }

    file.close();

    QDomNodeList tempList = mDomDocument.elementsByTagName("outline");
    int index = 0;
    QDomNode tempNode;
    while(index < tempList.count())
    {
        tempNode = tempList.at(index);
        if(aPageName == tempNode.toElement().attribute("title"))
        {
            break;
        }
        ++index;
        qDebug() << "index is  " << index;
    }


    QDomNodeList tempChildrenList = tempNode.childNodes();
    QDomNode tempChildNode;
    if(tempChildrenList.isEmpty())
    {
        return false;
    }

    index = 0;

    while(index < tempChildrenList.count())
    {
        tempChildNode = tempChildrenList.at(index);
        if(aItemName == tempChildNode.toElement().attribute("title"))
        {
            break;
        }
        ++index;
        qDebug() << "index is  " << index;
    }

    tempNode.removeChild(tempChildNode);

     if(file.exists())
     {
         file.remove();
     }

     if (!file.open(QFile::ReadWrite | QFile::Text)) {
         LOG_FORMAT("open fail:%s, %s", cfgfile.toLatin1().data(), file.errorString().toLatin1().data());
         return false;
     }

    QTextStream out(&file);
    mDomDocument.save(out, indentSize);
    file.close();

    MRFeedListModel* tempModel = getFeedListByFolderName(aPageName);
    tempModel->removeItem(aItemIndex);
    tempModel->refresh();

    return true;
}


// private method

void MrOpmlConfig::clearFeedList()
{
    LOG_METHOD;

    for (int i=0; i<mFeedModelList.count(); i++){
        if (MRFeedListModel* pd =  mFeedModelList.at(i)){
            delete pd;
        }
    }

    mFeedModelList.clear();
}

MRFeedListModel* MrOpmlConfig::feedList(const QString& folder) const
{
    LOG_METHOD;

    for (int i=0; i< mFeedModelList.count(); i++){
        MRFeedListModel* item = mFeedModelList.at(i);
        if (folder == item->folderName()){
            return item;
        }
    }
    return NULL;
}

QDomElement MrOpmlConfig::getFolderElement(const QString& folder)
{
    LOG_METHOD;

    QDomElement root = mDomDocument.documentElement();
    QDomElement child = root.firstChildElement("body");
    child = child.firstChildElement("outline");

    while (!child.isNull()){
        if (child.attribute("title") == folder){

            break;
        }
        child = child.nextSiblingElement("outline");
    }

    LOG_FORMAT("get folder element:%s, %d", folder.toLatin1().data(), child.isNull());
    return child;
}

void MrOpmlConfig::addConfigAttribute(QDomElement& domRoot)
{
    LOG_METHOD;
    domRoot.setAttribute("config", "MReader");
}

bool MrOpmlConfig::doSubscribed(QDomElement& domElement, bool subscribed)
{
    if (domElement.tagName() != "outline"){
        emit error(mrErrorStr[MR_ERR_BOURN]);
        return false;
    }
    domElement.setAttribute("subscribte", subscribed);

    return true;
}

void MrOpmlConfig::addFeedItem(MRFeedListModel* pNod, QDomElement& element)
{
    LOG_METHOD;
    FeedObject *item = new FeedObject(element.attribute("title", defaultValue),
                                      element.attribute("type", defaultTypeValue),
                                      element.attribute("xmlUrl", defaultValue),
                                      element.attribute("iconUrl", defaultValue),
                                      element.attribute("htmlUrl", defaultValue),
                                      element.attribute("text", defaultValue),
                                      element.attribute("subscribed", "true"),
                                      element.attribute("urllocalpath", defaultValue));

    // valiad the urlLocalpath here
    if (item->typeStr() == "rss" && item->urlLocalPathStr().isEmpty()){
        validLocalPath(item, element);
    }
    pNod->appendItem(item);
}

void MrOpmlConfig::validLocalPath(FeedObject *item, QDomElement& element)
{
    LOG_METHOD;
    QString localPath = QString();
    localPath = mLocalDir + localPath.setNum(getNextSerialNum(),16) + ".xml";
    item->setLocalPathStr(localPath);

    element.setAttribute("urllocalpath", localPath);

}

uint MrOpmlConfig::getNextSerialNum()
{
    LOG_METHOD;
    return mSerialNum++;
}


bool MrOpmlConfig::initFolderList(const QDomElement& domRoot)
{
    LOG_METHOD;
    if (domRoot.tagName() != "body") {
        emit error(mrErrorStr[MR_ERR_BOURN]);
        return false;
    }

    QDomElement child = domRoot.firstChildElement("outline");

    while (!child.isNull()){

        addFeedItem(mFolderListModel, child);

        LOG_FORMAT("title attribute:%s", child.attribute("title", defaultValue).toLatin1().data());

        child = child.nextSiblingElement("outline");
    }

    mFolderListModel->refresh();

    return true;
}

void MrOpmlConfig::parseModelName(const QDomElement& domRoot)
{
    LOG_METHOD;
    QDomElement head = domRoot.firstChildElement("head");

    QDomElement title = head.firstChildElement("title");

    mModelName = title.text();
}

void MrOpmlConfig::parseFolderElement(const QDomElement& element)
{

}

MRFeedListModel* MrOpmlConfig::getFolderListModel()
{
    LOG_METHOD;
    LOG_FORMAT("model list count:%d", mFolderListModel->count());
    return mFolderListModel;
}

bool MrOpmlConfig::addItemBySync(const QString& aFeedUrl, const QString& aFeedName,
                                 const QString& aPageName)
{
    LOG_METHOD;

    if(aFeedUrl.isEmpty() || aFeedName.isEmpty() || (aPageName.isEmpty()) )
    {
        return false;
    }

    qDebug() << aFeedUrl << "--" << aFeedName << "---" << aPageName;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (cfgfile.isEmpty())
        return false;

    QFile file(cfgfile);
    if (!file.open(QFile::ReadWrite | QFile::Text)) {
        LOG_FORMAT("open fail:%s, %s", cfgfile.toLatin1().data(), file.errorString().toLatin1().data());
        return false;
    }

    if (!mDomDocument.setContent(&file, true, &errorStr, &errorLine,
                           &errorColumn)){
        LOG_FORMAT("dom setcontent fail: %s", errorStr.toLatin1().data());
        return false;
    }

    file.close();

    QDomNodeList tempList = mDomDocument.elementsByTagName("outline");
    int index = 0;
    QDomNode tempNode;
    bool isFind = false;
    // find node by give pagename
    while(index < tempList.count())
    {
        tempNode = tempList.at(index);
        if(aPageName == tempNode.toElement().attribute("title"))
        {
            isFind = true;
            break;
        }
        ++index;
        qDebug() << "index is  " << index;
    }

    // page name has exist
    if(isFind)
    {
        qDebug() << "isFind is true";
        QDomElement temDomNod = mDomDocument.createElement("outline");
        temDomNod.setAttribute("text","text1");
        temDomNod.setAttribute("title",aFeedName);
        temDomNod.setAttribute("type","rss");
        temDomNod.setAttribute("xmlUrl",aFeedUrl);
        temDomNod.setAttribute("htmlUrl","www.sina.com.cn");
        tempNode.appendChild(temDomNod);

        MRFeedListModel* tempModel = getFeedListByFolderName(aPageName);
        addFeedItem(tempModel, temDomNod);
        tempModel->refresh();
    }
    else
    {
        qDebug() << "isFind is false";
        QDomNodeList tempList = mDomDocument.elementsByTagName("body");
        QDomNode tempNode = tempList.at(0);
        // make a new tempElement, and set attributes

        //folder item
        QDomElement tempFolder = mDomDocument.createElement("outline");
        tempFolder.setAttribute("text","New Feeds");
        tempFolder.setAttribute("title","New Feeds");
        tempFolder.setAttribute("type","folder");

        QDomElement temDomNod = mDomDocument.createElement("outline");

        temDomNod.setAttribute("text","text1");
        temDomNod.setAttribute("title",aFeedName);
        temDomNod.setAttribute("type","rss");
        temDomNod.setAttribute("xmlUrl",aFeedUrl);
        temDomNod.setAttribute("htmlUrl","www.sina.com.cn");

        tempFolder.appendChild(temDomNod);
        tempNode.appendChild(tempFolder);
    }

    //save to file. delete old file
    if(file.exists())
    {
        file.remove();
    }

    if (!file.open(QFile::ReadWrite | QFile::Text)) {
        LOG_FORMAT("open fail:%s, %s", cfgfile.toLatin1().data(), file.errorString().toLatin1().data());
        return false;
    }

    QTextStream out(&file);
    mDomDocument.save(out, indentSize);
    file.close();

    // update model.
//    MRFeedListModel* tempModel = getFeedListByFolderName(aPageName);
//    addFeedItem(tempModel, tempElement);
//    tempModel->refresh();
    loadOpml();
    return true;
}

QString MrOpmlConfig::getOpmlPath()
{
    LOG_METHOD;
    return cfgfile;
}

void MrOpmlConfig::loadOpml()
{
    LOG_METHOD;
//    if(NULL != mFolderListModel)
//    {
//        delete mFolderListModel;
//    }

//    mFolderListModel = new MRFeedListModel();
    mFolderListModel->clearItems();

    mrLoadXml();
}

//void MrOpmlConfig::removeOpml()
//{
//    LOG_METHOD;
//    if(NULL != mFolderListModel)
//    {
//        delete mFolderListModel;
//    }

//    mFolderListModel = new MRFeedListModel();

//    mrLoadXml();
//}
