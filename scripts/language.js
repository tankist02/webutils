var language="English"
//var language="Russian"

function display_text()
{
    document.write("<p><center>");

    if( language == "English" )
    {
        document.write(message_english);
    }
    else if( language == "Russian" )
    {
        document.write(message_russian);
    }

    document.write("</center>");
}
