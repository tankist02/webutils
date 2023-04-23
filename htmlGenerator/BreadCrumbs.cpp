#include "utils.h"
#include "BreadCrumbs.h"


void BreadCrumbs::loadTemplates(char const* dir)
{
    loadTemplateFile(dir, "bread_crumbs_link.html", linkFormat_);
}

std::string BreadCrumbs::getHtml() const
{
    if( empty() )
        return "";

    //char const* format = "/<a href=\"%s%s\">%s</a>";

    std::string res = "<b>Back / Назад: </b> ";

    for (Subdirs::const_iterator it = subdirs_.begin(); it != subdirs_.end(); ++it)
    {
        int dist = std::distance(it, subdirs_.end());
        std::string dl = formatDirLevel(dist);
        res += (boost::format(linkFormat_) % dl % *it % removeExt(*it)).str();
    }

    return res;
}

