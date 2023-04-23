#ifndef UTILS_H
#define UTILS_H


#include <string>


std::string removeExt(std::string const& fname);
std::string makeSubtitle(std::string const& subdir);
std::string makeTitle(std::string const& dir);
std::string formatDirLevel(int level);
int loadTemplateFile(char const* templ_dir, char const* const fname, std::string& templ);

#endif

