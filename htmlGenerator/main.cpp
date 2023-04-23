#include <cstdio>
#include <cstring>
#include <cctype>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdexcept>
#include <iostream>
#include <string>

#include <boost/format.hpp>

#include "utils.h"
#include "htmlGenerator.h"


static void usage(char const* const prog)
{
    std::cerr << "Usage: %s [<-td=templates_dir>] [-h]\n" << prog;
    std::cerr << "-td=template_dir - specify directory with html templates.\n";
    std::cerr << "-h - display this message.\n" << std::endl;
}

static std::string getRoot(char const* pname, char const* suffix)
{
	char const* p = strrchr(pname, '/');

    if( !p )
        return std::string("./") + suffix;

    return std::string(pname, p - pname + 1) + suffix;
}

int main(int argc, char* argv[])
{
    try
    {
        if( argc < 1 || argc > 4 )
        {
            usage(argv[0]);
            return -1;
        }

        std::string templ_dir = getRoot(argv[0], "templates");

        for( int i = 1; argv[i]; ++i )
        {
            if( strncmp(argv[i], "-td=", 4) == 0 )
            {
                templ_dir = argv[i] + 4;

                struct stat st;
                if( stat(templ_dir.c_str(), &st) != 0 )
                {
                    throw std::runtime_error((boost::format("Invalid template dir: %s, error: %s") % templ_dir % strerror(errno)).str());
                }
            }
            else if( strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 )
            {
                usage(argv[0]);
                return 1;
            }
            else
            {
                std::cerr << "Invalid option: " << argv[i] << std::endl;
                usage(argv[0]);
                return -1;
            }
        }

        HtmlGenerator hg(templ_dir.c_str());
        hg.process();
        hg.printStats();

        return 0;
    }
    catch( std::exception const& e )
    {
        std::cerr << "std::exception: " << e.what() << std::endl;
    }

    return -1;
}
