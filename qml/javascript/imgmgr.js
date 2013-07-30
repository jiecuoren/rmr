var picArray = new Array()
var rep=new RegExp("src=('|\"|)(.*?)('|\"|>| )","gim")

var repForTest = new RegExp("<.+?>","gim")

function findImg(aStr, aIndex)
{
    if("" === aStr)
    {
        return "";
    }

    var picArray= aStr.match(rep);
    var imgStr = picArray[aIndex];

    var returnStr = imgStr.substring(5, imgStr.length - 1);    
    return returnStr ;
}

function picsNumber(aDesStr, aContentStr) {
    var allStr =  aDesStr + aContentStr;
    var imgStr = allStr.match(rep);
    if (imgStr != null)
        return imgStr.length;
    else
        return 0;
}

function picsArray(aDesStr, aContentStr) {
    var allStr =  aDesStr + aContentStr;
    var imgStr = allStr.match(rep);
    picArray = null;
    picArray = imgStr;
}

function returnPicByIndex(aIndex, aDesStr, aContentStr){
    picsArray(aDesStr, aContentStr)

    var returnStr = picArray[aIndex].substring(5, picArray[aIndex].length - 1);
    console.log("returnStr is  " + returnStr);
    return returnStr;
}

function deleteHtmlText(aStr) {
    return aStr.replace(repForTest, "")
}



