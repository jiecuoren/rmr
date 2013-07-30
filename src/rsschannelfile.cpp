#include <QDateTime>
#include <QDebug>
#include <QDomDocument>
#include <QXmlStreamNamespaceDeclaration>
#include <QApplication>
#include <QDir>
//#include "dataprovider.h"
#include "rsschannelfile.h"
#include "mrdebug.h"
#include "mrdetailmodel.h"
#include "rssfiledownloader.h"

RSSChannelFile::RSSChannelFile( QObject *parent )
        : QObject(parent)
          ,mLoaded(false)
{
//    mXmlReader.setNamespaceProcessing(false);
    mRSSFileDownloader = RSSFileDownloader::openInstance();
    connect( mRSSFileDownloader, SIGNAL(requestCancel()), this, SIGNAL(loadingCancel()) );
}

RSSChannelFile::~RSSChannelFile()
{
    mRSSFileDownloader->disconnect(this);
    mRSSFileDownloader->closeInstance();
}

void RSSChannelFile::setSource( QString filepath )
{
    LOG_METHOD;

    if (mFilepath != filepath)
    {
        mFilepath = filepath;
        mFileHandle.setFileName(mFilepath);
        cleanup();
        mLoaded = preload();
    }
}

void RSSChannelFile::setSourceUrl(QString url, QString filepath)
{
    LOG_METHOD;

    mUrl = url;
    QString localPath = filepath;

    if("" == filepath)
    {
        QString appPath = qApp->applicationDirPath();
        QString xmlPath = appPath + "/xml";



        QString tempUrl = url;

        QString fileName = QString(tempUrl.toAscii().toBase64()).remove("/").remove("+") + ".xml";
        localPath = xmlPath + "/" + fileName;
    }
    setSource( localPath );
}


bool RSSChannelFile::preload()
{
    LOG_METHOD;

    cleanup();

    //pre-load content to a list of RSSItemShort
    QFile file(mFilepath);
    if (!file.exists()) return false;

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return false;
    }

    mXmlReader.setDevice(&file);

    while (mXmlReader.readNextStartElement()&& mXmlReader.name()!= "rss")
    {
        mXmlReader.skipCurrentElement();
    }

    if (mXmlReader.name() == "rss")
    {
        while (mXmlReader.readNextStartElement())
        {
            if (mXmlReader.name()!= "channel")
            {
                mXmlReader.skipCurrentElement();
            }
            else {
                while (!mXmlReader.hasError() && mXmlReader.readNextStartElement())
                {
                    if (mXmlReader.name()!="item")
                    {
                        mXmlReader.skipCurrentElement();
                    }
                    else {
                        //qDebug() << file.pos() <<"\n";
                        preloadItem();
                    }
                }
            }
        }
    }

    if (mXmlReader.hasError())
    {
        cleanup();
        //qDebug() << mXmlReader.errorString() << "\n";
        file.close();
        return false;
    }

    mXmlReader.clear();
    file.close();
    return true;
}


void RSSChannelFile::preloadItem()
{
    LOG_METHOD;

    RSSItemShort item;

    while (!mXmlReader.hasError() && mXmlReader.readNextStartElement())
    {
        qDebug()<<mXmlReader.qualifiedName().toString()<<" ";
        QString tagname = mXmlReader.qualifiedName().toString();

        if (tagname == "title")
        {
            item.setTitle( mXmlReader.readElementText().trimmed() );
        }
        else if (tagname == "guid")
        {
            item.mGuid = mXmlReader.readElementText().trimmed();
        }
        else if (tagname == "MXimg")
        {
            if (item.getIconPath() == "")
            {
                QXmlStreamAttributes attrs = mXmlReader.attributes();
                item.setIconPath(attrs.value("src").toString());
            }
            mXmlReader.skipCurrentElement();
        }
        else if (tagname == "description")
        {
            QString tempStr = mXmlReader.readElementText();
            //just keep a short paragraph
            item.mExcerpt = tempStr.left(140);
        }
        else if (tagname == "MXItemOffset")
        {
            item.mOffset = mXmlReader.readElementText().toInt();
        }
        else if (tagname == "pubDate")
        {
            item.setPubDate(mXmlReader.readElementText().trimmed() );
        }
        else {
            mXmlReader.skipCurrentElement();
        }
    }
    mItemList.append(item);
}


void RSSChannelFile::cleanup()
{
    mItemList.clear();
    mLoaded = false;
}

QListIterator<RSSItemShort>* RSSChannelFile::getItemListIterator()
{
    LOG_METHOD;

    if(mItemList.isEmpty())
    {
        return NULL;
    }
    else
    {
        QListIterator<RSSItemShort>* itor = new QListIterator<RSSItemShort>( mItemList );
        return itor;
    }
}


void RSSChannelFile::update()
{
    LOG_METHOD;

    if(!mUrl.isEmpty()) {
        qDebug() << "RSSChannelFile::update() mUrl is not empty, url is  " << mUrl;
        get(mUrl);
    }
}

void RSSChannelFile::get(QString url)
{
    LOG_METHOD;

    //fetch from mUrl
    emit loadingStartted();

    mRSSRequest = mRSSFileDownloader->getRSSFile(url);
    if(NULL != mRSSRequest)
    {
        connect( mRSSRequest, SIGNAL(finished(QByteArray)), this, SLOT(finished(QByteArray)) );
    }

}


int RSSChannelFile::merge(const QByteArray& data)
{
    LOG_METHOD;

    // -- Make a reader on incoming data --
    QXmlStreamReader xmlReader;
    xmlReader.setNamespaceProcessing(false);
    xmlReader.addData( data );

    closeFileHandler();
    // -- Make a writer and a backup file --
    QFile bakfile(mFilepath);
    if (mLoaded && bakfile.exists())
    {
        bakfile.rename(mFilepath+".bak"); //to be removed
    }

    QFile file(mFilepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        if (mLoaded && bakfile.exists()) bakfile.rename(mFilepath);
        xmlReader.clear();
        return 0;
    }

    QXmlStreamWriter xmlWriter(&file);

    // -- Start to analyze xml and merge data --
    int countNewItems = 0;
    countNewItems = processXmlFeeds( xmlReader, xmlWriter, bakfile );

    file.close();

    mItemList.clear();
    mItemList = mItemNewList;
    mItemNewList.clear();

    if (mLoaded && bakfile.exists())
    {
        bakfile.remove();
    }

    if (mItemList.count()>0) mLoaded = true;

    return countNewItems;
}


int RSSChannelFile::processXmlFeeds(QXmlStreamReader &reader, QXmlStreamWriter &writer, QFile& bakfile)
{
    LOG_METHOD;

    int newItemCount = 0;

    //-- Step 1: read all from reader and write into writer --
    mItemNewList.clear();
    reader.readNext();
    while (!reader.hasError() && !reader.isEndDocument())
    {
        qDebug() << reader.tokenString() << "\n";
        qDebug() << reader.qualifiedName().toString() <<"\n";
        if (reader.isStartElement() && reader.qualifiedName()=="item")
        {
            // -- the maximum number of items to store --
            if (mItemNewList.count() == 25)
            {
                reader.skipCurrentElement();
                continue;
            }
            else
            {
                RSSItemShort item;
                processXmlNewItem( reader, writer, item );
                mItemNewList.append( item );
            }
        }
        else if (reader.isStartElement() && reader.qualifiedName()=="link")
        {
            processHostName(reader, writer);
        }
        else {
            switch (reader.tokenType())
            {
            case QXmlStreamReader::StartDocument:
                writer.writeStartDocument();
                break;
            case QXmlStreamReader::EndDocument:
                writer.writeEndDocument();
                break;
            case QXmlStreamReader::StartElement:
            {
                writer.writeStartElement( reader.qualifiedName().toString() );
                QXmlStreamNamespaceDeclarations nsVector;
                nsVector = reader.namespaceDeclarations();
                for (int i=0; i<nsVector.count(); i++)
                {
                    writer.writeNamespace( nsVector[i].namespaceUri().toString(), nsVector[i].prefix().toString() );
                }
                writer.writeAttributes( reader.attributes() );
            }
                break;

            case QXmlStreamReader::EndElement:
                if ( !(reader.qualifiedName()=="rss" || reader.qualifiedName()=="channel") )
                    writer.writeEndElement();
                break;
            case QXmlStreamReader::Characters:
                if (reader.isCDATA())
                    writer.writeCDATA( reader.text().toString() );
                else
                    writer.writeCharacters( reader.text().toString() );
                break;
            default:
                break;
            }
        }   // end of else

        reader.readNext();
    }   // end of while

    //-- Step 2: start to merge two files and two lists --
    newItemCount = mItemNewList.count();

    if (mLoaded && mItemNewList.count()<25)
    {
        // merge starts
        newItemCount = mItemNewList.count();
        if (bakfile.open(QFile::ReadOnly | QFile::Text))
        {
            for (int i=0; i<mItemList.count() && mItemNewList.count() <25; i++)
            {
                bool redundant = false;
                for (int j=0; j<mItemNewList.count(); j++)
                {
                    if (mItemList[i].mGuid !="" && mItemList[i].mGuid == mItemNewList[j].mGuid)
                        redundant = true;

                    if ( mItemList[i].mGuid == mItemNewList[j].mGuid
                            && mItemList[i].mTitle == mItemNewList[j].mTitle )
                            //&& mItemList[i].mPubDate == mItemNewList[j].mPubDate)
                    {
                        redundant = true;
                        newItemCount --;
                    }

                    if (redundant) break;
                }

                if (!redundant)
                {
                    bakfile.seek(mItemList[i].mOffset);
                    reader.clear();
                    reader.setDevice(&bakfile);
                    reader.setNamespaceProcessing(false);
                    processXmlOldItem( reader, writer, mItemList[i] );
                    mItemNewList.append(mItemList[i]);
                }
            }
        }
    }

    writer.writeEndElement();
    writer.writeEndElement();
    writer.writeEndDocument();
    return newItemCount;
}

void RSSChannelFile::processXmlOldItem (QXmlStreamReader &reader, QXmlStreamWriter &writer, RSSItemShort &item)
{
    LOG_METHOD;

    int offset = writer.device()->pos();
//    writer.writeStartElement( "item" );

    reader.readNext();
    while (!reader.hasError()&&!(reader.isEndElement() && reader.qualifiedName().toString()=="item"))
    {
        qDebug()<<reader.tokenString() << "\n";
        qDebug()<<reader.name().toString()<< "\n";
        switch (reader.tokenType())
        {
        case QXmlStreamReader::StartElement:
           {
                writer.writeStartElement( reader.qualifiedName().toString() );
                QXmlStreamNamespaceDeclarations nsVector;
                nsVector = reader.namespaceDeclarations();
                for (int i=0; i<nsVector.count(); i++)
                {
                    writer.writeNamespace( nsVector[i].namespaceUri().toString(), nsVector[i].prefix().toString() );
                }
                writer.writeAttributes( reader.attributes() );

                if (reader.qualifiedName().toString() == "MXItemOffset")
                {
                    QString strOffset;
                    writer.writeCharacters(strOffset.setNum(offset));
                    writer.writeEndElement();
                    reader.skipCurrentElement();
                }
            }
            break;

        case QXmlStreamReader::EndElement:
            writer.writeEndElement();
            break;

        case QXmlStreamReader::Characters:
            if (reader.isCDATA())
                writer.writeCDATA( reader.text().toString() );
            else
                writer.writeCharacters( reader.text().toString() );
            break;

        default:
            break;
        }

        reader.readNext();
    }
    writer.writeEndElement(); //item

    item.mOffset = offset;
}

void RSSChannelFile::processXmlNewItem(QXmlStreamReader &reader, QXmlStreamWriter &writer, RSSItemShort& item)
{
    LOG_METHOD;
    Q_ASSERT (reader.name().toString() == "item");

    QString title = "";
    QString pubdate = "";
    QString content="";
    QString guid = "";

    int offset = writer.device()->pos();
    writer.writeStartElement( "item" );

    while (!reader.hasError() && reader.readNextStartElement())
    {
        QString tagname = reader.qualifiedName().toString();

        writer.writeStartElement( tagname );
        QXmlStreamNamespaceDeclarations nsVector;
        nsVector = reader.namespaceDeclarations();
        for (int i=0; i<nsVector.count(); i++)
        {
            writer.writeNamespace( nsVector[i].namespaceUri().toString(), nsVector[i].prefix().toString() );
        }
        writer.writeAttributes( reader.attributes() );

        //-- copy all between start-tag and end-tag --
        QString temp = "";
        bool isCData = false;
        reader.readNext();
        while ( !(reader.isEndElement() && tagname==reader.qualifiedName()) )
        {
            if (reader.tokenType() == QXmlStreamReader::Characters)
            {
                isCData = isCData || reader.isCDATA();
                temp += reader.text().toString();
            }
            reader.readNext();
        }

        if(tagname == "description" && content =="") {
            content = temp;
            removeEmbedContent(temp);
        }
        else if (tagname == "content:encoded") {
            content = temp;
            removeEmbedContent(temp);
        }
        else if (tagname == "title") {
            title = temp.trimmed();
        }
        else if (tagname == "pubDate") {
            pubdate = temp;
        }
        else if (tagname == "guid") {
            guid = temp.trimmed();
        }

        if (isCData)
            writer.writeCDATA( temp );
        else
            writer.writeCharacters( temp );

        writer.writeEndElement();
    }

    // -- extract Image Links from txtDesc --
    QStringList imgUrls = extractImagePaths( content );
    for (int i=0; i<imgUrls.count(); i++)
    {
        writer.writeStartElement("MXimg");
        writer.writeAttribute( "src", imgUrls[i] );
        writer.writeEndElement();
    }

    // -- write offset --
    writer.writeStartElement("MXItemOffset");
    writer.writeCharacters( QString::number(offset) );
    writer.writeEndElement();

    // -- end of writing </item> --
    writer.writeEndElement();

    // -- return data in item --
    item.setTitle( title );
    item.setPubDate( pubdate );
    item.mExcerpt = content.left(140);
    item.mGuid = guid;
    item.mOffset = offset;

    if (imgUrls.count() > 0)
    {
        item.setIconPath( imgUrls[0] );
    }
}
void RSSChannelFile::processHostName(QXmlStreamReader& reader, QXmlStreamWriter& writer)
{
    LOG_METHOD;
    Q_ASSERT (reader.name().toString() == "link");

    writer.writeStartElement("link");

    QString tmpUrl = reader.readElementText().trimmed();

    mHostName = getHostName(tmpUrl);

    writer.writeCharacters(tmpUrl);
    writer.writeEndElement();
}

QString RSSChannelFile::getHostName(QString url)
{
    QString hostName = "";

    QStringList tmpList = url.split( ".", QString::SkipEmptyParts);
    if (tmpList.count() < 1)
        return "";

    int i = 0;
    hostName = tmpList.at(i);
    while ( ++i < tmpList.count()) {
        if ( tmpList.at(i).toLower().contains("com")
             || tmpList.at(i).toLower().contains("net")
             || tmpList.at(i).toLower().contains("edu")
             || tmpList.at(i).toLower().contains("org")
             || tmpList.at(i).toLower().contains("cn"))
            break;

        hostName = tmpList.at(i);
    }

    return hostName;
}

bool RSSChannelFile::getFullItemlist(QList<DetailItem*>& itemList)
{
    LOG_METHOD;

    for (int i=0; i<mItemList.count(); i++)
    {
        DetailItem* item = new DetailItem(this);

        item->setIndex(mItemList.count());
        item->setOffset(mItemList[i].mOffset);
        item->setTitle(mItemList[i].mTitle);
        item->setIconPath(mItemList[i].mIconPath);
        item->setPubDate(mItemList[i].mPubDate);

        itemList.append(item);
    }
    return true;
}

void RSSChannelFile::removeEmbedContent(QString& content)
{

    QRegExp rx("<img[^>]*>[^<>]*", Qt::CaseInsensitive);
    QString rp = "<i><u><font color=blue>picture</font></u></i>";
    content.replace(rx, rp);
    rx.setPattern("<embed[^>]*>[^<>]*");
    rp = "<i><u><font color=blue>multimedia</blue></u></i>";
    content.replace(rx, rp);
}

QStringList RSSChannelFile::extractImagePaths(QString text)
{
    LOG_METHOD;

//    QStringList imgUrls;
//    QRegExp rx("<img[^>]*>[^<>]*");
//    rx.setPatternSyntax(QRegExp::RegExp);
//    QRegExp src("\\bhttp\.*\\b'");
//    int pos = 0;

//    while ((pos = rx.indexIn(text, pos)) != -1) {
//        QString tmp = rx.cap(1);
//        LOG_FORMAT("tmp: %s", tmp.toLatin1().data());
//        src.indexIn(tmp, 0);
//        imgUrls << src.cap(1);
//        pos += rx.matchedLength();
//    }

//    LOG_FORMAT("extractImagePaths: %d", imgUrls.count());

    QStringList imgUrls;

    if (text.trimmed().isEmpty()) return imgUrls;

    QStringList tempList = text.split( "<img ", QString::SkipEmptyParts, Qt::CaseInsensitive);
    QRegExp rx("[\"'](http://\\S+)[\"']", Qt::CaseInsensitive);
    QString tempStr = "";
    // extract 9 URLs at most
    for (int i=1; i<10 && i<tempList.count(); i++)
    {
        QString rxStr = tempList.at(i);
        rx.indexIn(rxStr.left(rxStr.indexOf("/>")));
        tempStr = rx.cap(1);

        if (isValidUrl(tempStr))
        {
            imgUrls.append(tempStr);
        }
    }
    return imgUrls;
}

bool RSSChannelFile::isValidUrl(QString url)
{
    LOG_METHOD;

    if(url.isEmpty())
    {
        return false;
    }

    QStringList tempList = url.split( ".", QString::SkipEmptyParts);

    if(tempList.isEmpty())
    {
        return false;
    }

    QString subStr = tempList.at(tempList.count() - 1);

    QString hostName = getHostName(url);

    return (hostName.contains(mHostName)
            || 0 == QString::compare("jpg", subStr, Qt::CaseInsensitive)
            || 0 == QString::compare("png", subStr, Qt::CaseInsensitive)
            || 0 == QString::compare("gif", subStr, Qt::CaseInsensitive))
            && userDefined(url);
}

bool RSSChannelFile::userDefined(QString url)
{
    // should read these rules from INI, later

    return !(url.contains("avatar")
             || url.contains("Tao123")
             || url.contains("post_label")
             || url.contains("mailthis", Qt::CaseInsensitive)
             || url.contains("a2.img")
             || url.contains("bookmark", Qt::CaseInsensitive)
             || url.contains("mf.gif")
             || url.contains("eweekly")
             || url.contains("nprapidev"))
            || url.contains("ezhoukan"); // should removed when image provider can manage wrong url;
}

void RSSChannelFile::finished(QByteArray aData)
{
    LOG_METHOD;

    if(aData.isEmpty())
    {
        emit newItemsArrived(0);
        return;
    }

    int countNewItems = merge (aData);
    emit newItemsArrived( countNewItems );
}


void RSSChannelFile::cancelAllRequest()
{
    LOG_METHOD;

    if(NULL != mRSSFileDownloader)
    {
        mRSSFileDownloader->cancelAllRequest();
    }
}

bool RSSChannelFile::openFileHandler()
{   
    if (mFileHandle.handle() != -1)
        mFileHandle.close();

    if (!mFileHandle.open(QIODevice::ReadOnly | QIODevice::Text)){
        return false;
    }

    return true;
}

void RSSChannelFile::closeFileHandler()
{
    if (mFileHandle.handle() != -1)
        mFileHandle.close();
}

bool RSSChannelFile::doConstructItem(QXmlStreamReader& reader, DetailItem& detailItem)
{
    reader.readNextStartElement();

    while (!reader.hasError() && !(reader.isEndElement() && reader.qualifiedName().toString()=="item"))
    {
        QString tagName = reader.qualifiedName().toString();

        qDebug()<<reader.tokenString()<<':'<<tagName<<"\n";

        if (tagName == "MXimg")
        {
            QXmlStreamAttributes attrs = reader.attributes();
            QString imageurl = attrs.value("src").toString();

            detailItem.appendImageList(imageurl);
            reader.skipCurrentElement();
        }
        else if (tagName == "description" && detailItem.description().isEmpty())
        {
            detailItem.setDescription(reader.readElementText().trimmed());
        }
        else if (tagName == "content:encoded")
        {
            detailItem.setDescription(reader.readElementText().trimmed());
        }
        else if (tagName == "pubDate")
        {
            detailItem.setPubDate(reader.readElementText().trimmed() );
        }
        else if (tagName == "link")
        {
            detailItem.setLink(reader.readElementText().trimmed());
        }

        reader.readNextStartElement();
    }

    return true;
}

bool RSSChannelFile::constructItem(DetailItem& detailItem, int byteStart)
{
    if (mFileHandle.handle() == -1)
        openFileHandler();

    mFileHandle.seek(byteStart);

    QXmlStreamReader reader(&mFileHandle);
    reader.setNamespaceProcessing(false);

    return doConstructItem(reader, detailItem);
}
