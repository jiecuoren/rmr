#ifndef _COMMON_STRINGS
#define _COMMON_STRINGS

const int KRequestTimeOut = 60 * 1000; // 60 s
const QString KNetworkError = "Network Error!";
const QString KUserInfoError = "Authorization Failed!";
const QString KFileError = "File error!";
const QString KParameterError = "Wrong Parameter Passed!";

const QString KAuthHeaderKey = "Authorization";
const QString KAuthHeaderValue = "Basic ";

const QString KShareBlogUrl = "http://api.t.sohu.com/statuses/update.xml";

const QString KPostfixIcon = "_icon";
const int KIconWidth = 120;
const QString KPostfixClip150150 = "_clip_150150";
const int K150150Width = 150;
const QString KPostfixClip320150 = "_clip_320150";
const int K320150Width = 320;
const int K320150Height = 150;

const int KMaxItemsInHorizontalView = 20;
const int KMaxImgUrlCount = 4;

const QString KDefaultIcon = ":/resources/icon/update.png";

enum ImageType
{
    EIcon = 0,
    //single pic in detail view, show original pic
    EOriPic,
    //2 or 3 pics in detail view.
    E150150,
    //3 pics in detail view, first image.
    E320150
};


#endif /* _COMMON_STRINGS */
