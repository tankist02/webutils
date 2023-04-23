#include <iostream>
#include <string>
#include <set>
#include <cstring>
#include <cctype>
#include <limits.h>

// Algorithm:
// Starting from end of the path find the slash '/' separator
// Examine and convert if necessary the name from the found '/' (or start) and next '/' (or EOL)
//

static bool good_char(int ch)
{
    /*if( isalpha(ch) ||
        isdigit(ch) ||
        ch == '-' ||
        ch == '.' )
    {
        return true;
    }

    return false;*/

    if( ch == '\'' ||
        ch == '"' ||
        ch == ' ' )
    {
        return false;
    }

    return true;
}

static int fix_name(char* name)
{
    if( !name || !*name )
    {
        return 0;
    }

    int changes = 0;

	for( char* p = name + strlen(name) - 1; p >= name; --p )
	{
		if( !good_char(*p) )
		{
			*p = '_';
			++changes;
		}
	}

	return changes;
}

static int rename_path(char* old_path, char* new_path, char* start)
{
    /*std::cout
        << "\nrename_path:"
        << "\nold_path: '" << old_path << "'"
        << "\nnew_path: '" << new_path << "'"
        << "\nstart: '" << start << "'"
        << std::endl;*/

    if( fix_name(start) > 0 )
    {
        printf("%s --> %s\n", old_path, new_path);
        rename(old_path, new_path);
    }

    if( start > new_path )
    {
        --start;
    }

    *start = 0;
    old_path[start - new_path] = 0;

    return 0;
}

static bool already_done(char const* path)
{
    typedef std::set< std::string > Set;

    static Set set;

    Set::const_iterator it = set.find(path);

    if( it == set.end() )
    {
        set.insert(path);
        return false;
    }

    return true;
}

static int process_path(char* path)
{
    if( !path || !*path )
    {
        return -1;
    }

    size_t len = strlen(path);

    char buf[PATH_MAX] = { 0 };
    strncpy(buf, path, sizeof(buf) - 1);

    for( char* p = buf + len - 1; p >= buf; --p )
    {
        if( already_done(p) )
        {
            break;
        }

        if( *p == '/' )
        {
            rename_path(path, buf, p + 1);
        }
        else if( p == buf )
        {
            rename_path(path, buf, p);
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    for( size_t i = 1; argv[i]; ++i )
	{
		process_path(argv[i]);
	}

	return 0;
}

