#ifndef HTML_GENERATOR_H
#define HTML_GENERATOR_H


#include <string>
#include <vector>

#include "BreadCrumbs.h"


class HtmlGenerator
{
public:
    HtmlGenerator(
        char const* const templ_dir);

    ~HtmlGenerator()
    {
    }

    int process();
    int printStats();

protected:
    typedef std::vector< std::string > Images;
    size_t loadImages(char const* const mask);

    void loadTemplates();

    int generateLinkFile(
        Images::const_iterator it,
        std::string const& html_name,
        std::string const& index_name);

    std::string processDir();
    int processSubdirs(std::string const& html_up);

protected:
    Images images;

    std::string templDir;

    std::string top_level_prologue;
    std::string prologue;
    std::string epilogue;

    std::string dirData;

    std::string previewLink;

    std::string singlePage;
    std::string firstPage;
    std::string lastPage;
    std::string middlePage;

    int imagesCount;
    int subDirCount;
    int indexCount;
    int linkCount;

    BreadCrumbs breadCrumbs;

    bool showForms;
};


#endif

