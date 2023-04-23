// Global variables with default values
var DEFAULT_LANGUAGE = "english";
var DEFAULT_SIZE = "large";

var languageGlob = DEFAULT_LANGUAGE;
var sizeGlob = DEFAULT_SIZE;

function clickLanguage(myRadio) 
{
    languageGlob = myRadio.value;
    var s = window.location.href;
    s = s.substring(0, s.indexOf('?'));
    window.location = s + "?language=" + languageGlob;
    window.location.refresh();
}

function clickImageSize(myRadio) 
{
    sizeGlob = myRadio.value;
}

function clickPreview(link)
{
    getVariables();
    window.location = link.id + ".html?language=" + languageGlob + "&size=" + sizeGlob;
}

function clickLink(link)
{
    getVariables();
    window.location = link.id + "?language=" + languageGlob + "&size=" + sizeGlob;
}

// Displays text under photo in selected language
function displayText()
{
    getQueryVariables();

    document.write("<p><center>");

    if (languageGlob == "english")
    {
        document.write(message_english);
    }
    else if (languageGlob == "russian")
    {
        document.write(message_russian);
    }

    document.write("</center>");
}

// Displays summary text on the firs page
function displaySummary()
{
    getQueryVariables();

    document.write("<font color='yellow' size='5'>");
    //document.write("<mark>");
    if (languageGlob == "english")
    {
        document.write(summary_english);
    }
    else if (languageGlob == "russian")
    {
        document.write(summary_russian);
    }
    //document.write("</mark>");
    document.write("</font>");
}

// Gets variable from URL
function getQueryVariable(variable)
{
    var query = window.location.search.substring(1);
    var vars = query.split("&");
    for (var i = 0; i < vars.length; i++) 
    {
        var pair = vars[i].split("=");
        if (pair[0] == variable)
        {
            return pair[1];
        }
   }
   return "";
}

function printLanguageForm()
{
    document.write(' \
        <b>Select your language / Выберите язык:</b> \
        <form action="" id="languageFormID" name="languageForm"> \
            <input type="radio" id="englishID" name="language" onclick="clickLanguage(this);" value="english">English / Английский<br> \
            <input type="radio" id="russianID" name="language" onclick="clickLanguage(this);" value="russian">Russian / Русский<br> \
        </form>');

    languageGlob = getQueryVariable("language");

    if (languageGlob == "english" || languageGlob == "") // english is default
    {
        document.forms["languageFormID"]["englishID"].checked = true;
    }
    else
    {
        document.forms["languageFormID"]["russianID"].checked = true;
    }
}

function printSizeForm()
{
    document.write(' \
        <b>Select image size / Выберите размер фоток:</b><br> \
        <form action="" id="imageSizeFormID" name="imageSizeForm"> \
            <input type="radio" id="smallID" name="imageSize" onclick="clickImageSize(this);" value="small">Small / Малый (800)<br> \
            <input type="radio" id="mediumID" name="imageSize" onclick="clickImageSize(this);" value="medium">Medium / Средний (1300)<br> \
            <input type="radio" id="largeID" name="imageSize" onclick="clickImageSize(this);" value="large">Large / Большой (2400)<br> \
            <input type="radio" id="originalID" name="imageSize" onclick="clickImageSize(this);" value="jpg">Original / Оригинал<br> \
        </form> \
        <br>');

    sizeGlob = getQueryVariable("size");
 
    if (sizeGlob == "small")
    {
        document.forms["imageSizeFormID"]["smallID"].checked = true;
    }
    else if (sizeGlob == "medium")
    {
        document.forms["imageSizeFormID"]["mediumID"].checked = true;
    }
    else if (sizeGlob == "large" || sizeGlob == "") // large is default
    {
        document.forms["imageSizeFormID"]["largeID"].checked = true;
    }
    else if (sizeGlob == "jpg") // .jpg is extension for original files
    {
        document.forms["imageSizeFormID"]["originalID"].checked = true;
    }
}

function getVariables()
{
    if (languageGlob == "")
        languageGlob = DEFAULT_LANGUAGE;

    if (sizeGlob == "")
        sizeGlob = DEFAULT_SIZE;
}

function getQueryVariables()
{
    languageGlob = getQueryVariable("language");

    if (languageGlob == "")
        languageGlob = DEFAULT_LANGUAGE;

    sizeGlob = getQueryVariable("size");

    if (sizeGlob == "")
        sizeGlob = DEFAULT_SIZE;
}

