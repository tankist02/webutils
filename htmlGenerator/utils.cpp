#include <cassert>
#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>

#include "utils.h"


// Creates title for dir
std::string makeTitle(std::string const& dir)
{
    // skipt leading ./ if present
    std::string::size_type pos = 0;
    if( dir.size() > 1 && dir[0] == '.' && dir[1] == '/' )
        pos = 2;

    std::string title = dir.substr(pos);

    std::replace_if(title.begin(), title.end(), std::bind2nd(std::equal_to< char >(), '_'), ' ');
    assert(!title.empty());

    // Capitalize the first letter and all after white-space
    for( std::string::iterator it = title.begin(); it != title.end(); ++it )
    {
        if( it == title.begin() || *(it - 1) == ' ' )
            *it = toupper(*it);
    }

    return title;
}

// Creates title for subdir
std::string makeSubtitle(std::string const& subdir)
{
    std::string subtitle;

    // Skip leading ./
    if( subdir.size() > 1 && subdir[0] == '.' && subdir[1] == '/' )
        subtitle = subdir.substr(2);
    else
        subtitle = subdir;

    std::replace_if(subtitle.begin(), subtitle.end(), std::bind2nd(std::equal_to< char >(), '_'), ' ');

    return subtitle;
}

std::string removeExt(std::string const& fname)
{
    std::string res;
    std::string::size_type pos = fname.rfind('.');
    if( pos == std::string::npos )
        res = fname;
    else
        res = fname.substr(0, pos);

    return res;
}

std::string formatDirLevel(int level)
{
    if( level == 0 )
        return "./";

    std::string res;

    for( int i = 0; i < level; ++i )
    {
        res += "../";
    }

    return res;
}

int loadTemplateFile(char const* templ_dir, char const* const fname, std::string& templ)
{
    std::cout << "Loading HTML template from dir: " << templ_dir << ", fname: " << fname << std::endl;
    std::string fpath = templ_dir;
    fpath += fname;

    std::ifstream fin(fpath.c_str());
    assert(fin);

    templ.reserve(1024);
    templ.resize(0);
    std::string line;

    while (std::getline(fin, line))
    {
        templ += line;
        templ += "\n";
    }

    std::cout << "Loaded HTML template from file: " << fpath << std::endl;

    return 0;
}

