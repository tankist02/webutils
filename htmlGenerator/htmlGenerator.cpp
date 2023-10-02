#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <jpeglib.h>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "utils.h"
#include "htmlGenerator.h"

namespace fs = boost::filesystem;
namespace al = boost::algorithm;

HtmlGenerator::HtmlGenerator(
    char const* const templ_dir)
    : templDir(templ_dir)
    , imagesCount(0)
    , subDirCount(0)
    , indexCount(0)
    , linkCount(0)
    , showForms(true)
{
    std::replace_if(templDir.begin(), templDir.end(), std::bind(std::equal_to<char>(), std::placeholders::_1, '\\'), '/');

    int len = templDir.length();
    if( len > 0 && templDir[len - 1] != '/' )
        templDir += '/';

    loadTemplates();
}

void HtmlGenerator::loadTemplates()
{
    loadTemplateFile(templDir.c_str(), "top_level_prologue_templ.html", top_level_prologue);
    loadTemplateFile(templDir.c_str(), "prologue_templ.html", prologue);
    loadTemplateFile(templDir.c_str(), "epilogue_templ.html", epilogue);

    loadTemplateFile(templDir.c_str(), "dirData_templ.html", dirData);

    loadTemplateFile(templDir.c_str(), "preview_templ.html", previewLink);

    loadTemplateFile(templDir.c_str(), "singlePage_templ.html", singlePage);
    loadTemplateFile(templDir.c_str(), "firstPage_templ.html", firstPage);
    loadTemplateFile(templDir.c_str(), "lastPage_templ.html", lastPage);
    loadTemplateFile(templDir.c_str(), "middlePage_templ.html", middlePage);

    breadCrumbs.loadTemplates(templDir.c_str());
}

size_t HtmlGenerator::loadImages(char const* const ext)
{
    images.resize(0);

    size_t ext_len = strlen(ext);

    for (fs::directory_iterator d("."); d != fs::directory_iterator(); ++d)
    {
        std::string const& image_name = d->path().string();

        // Load only jpeg files that end in .jpg
        if (al::ends_with(image_name, ext))
        {
            // Store base name without extension
            images.push_back(image_name.substr(0, image_name.size() - ext_len));

            ++imagesCount;
        }
    }

    return images.size();
}

char const* getOrientation(char const* image_file)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE* fin = fopen(image_file, "rb");
    
    if (!fin)
    {
        std::cerr << "Failed to open jpeg file: " << image_file << ", error: " << strerror(errno) << std::endl;
        return NULL;
    }

    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error(&jerr);

    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress(&cinfo);

    /* this makes the library read from infile */
    jpeg_stdio_src(&cinfo, fin);

    /* reading the image header which contains image information */
    jpeg_read_header(&cinfo, true);

    fclose(fin);

    // 1.78 is ratio of widescreen monitor (1920x1080)
    // Photo is usually ratio 3x2
    if ((double)cinfo.image_width > 1.78 * (double)cinfo.image_height)
        return "width='92";

    return "height='92";
}

int HtmlGenerator::generateLinkFile(
    Images::const_iterator it, // Image file name without extension
    std::string const& html_name, // Link file name being generated
    std::string const& index_name) // Link to upper-level page with thumbnails
{
    std::ofstream f(html_name.c_str());
    if (!f)
    {
        std::stringstream ss;
        ss << "Failed to open file: " << html_name
            << ", error: " << strerror(errno);
        throw std::runtime_error(ss.str());
    }

    ++linkCount;

    std::cout << "Generating link file: " << html_name << std::endl;

    std::string image_name = *it + ".jpg";
    char const* orientation = getOrientation(image_name.c_str()); // Depending on image being horiz or vert

    if (images.size() == 1)
    {
        f << boost::format(singlePage)
            % *it
            % *it
            % index_name
            % *it
            % orientation;
    }
    else if (it == images.begin())
    {
        f << boost::format(firstPage)
            % *it
            % *it
            % index_name
            % *(it + 1)
            % *it
            % orientation;
    }
    else if( (it + 1) == images.end() )
    {
        f << boost::format(lastPage)
            % *it
            % *it
            % *(it - 1)
            % index_name
            % *it
            % orientation;
    }
    else
    {
        f << boost::format(middlePage)
            % *it
            % *it
            % *(it - 1)
            % index_name
            % *(it + 1)
            % *it
            % orientation;
    }

    return 0;
}

int HtmlGenerator::process()
{
    std::string up = processDir();
    processSubdirs(up);

    return 0;
}

std::string HtmlGenerator::processDir()
{
    char fulldir[PATH_MAX];
    if( !getcwd(fulldir, sizeof(fulldir)) )
    {
        std::cerr << "Failed to getcwd, error: " << strerror(errno) << std::endl;
        exit(-1);
    }
    std::replace_if(fulldir, fulldir + strlen(fulldir), std::bind(std::equal_to<char>(), std::placeholders::_1, '\\'), '/');
    const char* p = strrchr(fulldir, '/');

    std::string index_name = p + 1;

    std::string title = makeTitle(index_name);

    index_name += ".html";

    std::cout << "Generating index file: " << index_name << std::endl;

    ++indexCount;

    std::ofstream fout(index_name.c_str());

    if (showForms) // Print forms for language and size only at the top-level page
    {
        fout << boost::format(top_level_prologue) % title % title;
        showForms = false;
    }
    else
    {
        fout << boost::format(prologue) % title % title;
    }

    if (!breadCrumbs.empty())
    {
        fout << "\n" << breadCrumbs.getHtml() << "<br><p>\n\n";
    }

    std::vector<std::string> dirs;

    for (fs::directory_iterator d("."); d != fs::directory_iterator(); ++d)
    {
        if (fs::is_directory(*d))
        {
            std::string const& subdir = d->path().string();
            dirs.push_back(subdir);
        }
    }

    std::sort(dirs.begin(), dirs.end());

    bool first = true;
   
    for (size_t i = 0; i < dirs.size(); ++i)
    {
        std::string const& subdir = dirs[i];
        std::cout << "Creating link to subdir: " << subdir << std::endl;

        ++subDirCount;

        if (first)
        {
            fout << "<p><b>Forward / Вперед:</b>\n";
            first = false;
        }

        std::string subtitle = makeSubtitle(subdir);
        fout << boost::format(dirData)
            % subdir
            % subdir
            % subtitle;
    }

    loadImages(".jpg");

    std::sort(images.begin(), images.end());

    for (Images::const_iterator it = images.begin(); it != images.end(); ++it)
    {
        fout << boost::format(previewLink)
            % *it
            % *it;

        std::string html_name = *it + ".html";
        generateLinkFile(it, html_name, index_name);
    } // loop for all images in the current dir

    fout << epilogue;

    return index_name;
}

int HtmlGenerator::processSubdirs(std::string const& html_up)
{
    for (fs::directory_iterator d("."); d != fs::directory_iterator(); ++d)
    {
        if (fs::is_directory(*d))
        {
            std::string const& subdir = d->path().string();

            std::cout << "Going into subdir: " << subdir << std::endl;

            // recurse into subdirs
            if (chdir(subdir.c_str()) != 0)
            {
                std::cerr << "Failed to changed to subdir: " << subdir << ", error: " << strerror(errno) << std::endl;
                continue;
            }

            breadCrumbs.push(html_up);

            process();

            if (chdir("..") != 0)
            {
                std::cerr << "Failed to chdir(..), error: " << strerror(errno) << std::endl;
                exit(-1);
            }

            breadCrumbs.pop();
        }
    }

    return 0;
}

int HtmlGenerator::printStats()
{
    std::cout << "\nStats:\nImages: " << imagesCount << ", subdirs: " << subDirCount << std::endl;
    std::cout << "Index files: generated: " << indexCount << std::endl;
    std::cout << "Link files: generated: " << linkCount << std::endl;

    return 0;
}

