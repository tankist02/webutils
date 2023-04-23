#include <cstdio>
#include <iostream>
#include <string>
#include <set>
#include <cstring>
#include <cctype>
#include <cerrno>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <utf8.h>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

// Comment out to disable debug printing
//#define DEBUG_PRINT

#ifdef DEBUG_PRINT
#define PRINT(m) std::cout << m << " (" << __FILE__ << ":" << __LINE__ << ")\n";
#else
#define PRINT(m)
#endif

bool dry_run = false;


// Algorithm:
// Starting from a given directory
// Do depth-first recursive search
// Rename entries where appropriate
//
// You may want to run chmod -R u+w 
// on the dirs to process beforehand

static bool isGood(int ch)
{
    /* ASCII table
	  0 NUL	1 SOH	 2 STX	  3 ETX	   4 EOT    5 ENQ    6 ACK    7	BEL
      8 BS	9 HT	10 NL	 11 VT	  12 NP	   13 CR    14 SO    15	SI
      16 DLE   17 DC1	18 DC2	 19 DC3	  20 DC4   21 NAK   22 SYN   23	ETB
      24 CAN   25 EM	26 SUB	 27 ESC	  28 FS	   29 GS    30 RS    31	US
      32 SP    33  !	34  "	 35  #	  36  $	   37  %    38	&    39	 '
      40  (    41  )	42  *	 43  +	  44  ,	   45  -    46	.    47	 /
      48  0    49  1	50  2	 51  3	  52  4	   53  5    54	6    55	 7
      56  8    57  9	58  :	 59  ;	  60  <	   61  =    62	>    63	 ?
      64  @    65  A	66  B	 67  C	  68  D	   69  E    70	F    71	 G
      72  H    73  I	74  J	 75  K	  76  L	   77  M    78	N    79	 O
      80  P    81  Q	82  R	 83  S	  84  T	   85  U    86	V    87	 W
      88  X    89  Y	90  Z	 91  [	  92  \	   93  ]    94	^    95	 _
      96  `    97  a	98  b	 99  c	 100  d	  101  e   102	f   103	 g
     104  h   105  i   106  j	107  k	 108  l	  109  m   110	n   111	 o
     112  p   113  q   114  r	115  s	 116  t	  117  u   118	v   119	 w
     120  x   121  y   122  z	123  {	 124  |	  125  }   126	~   127	DEL
    */

	if (
		ch == '-' ||
		ch == '.' ||
		(ch >= '0' && ch <= '9') ||
		(ch >= 'A' && ch <= 'Z') ||
		ch == '_' ||
		(ch >= 'a' && ch <= 'z'))
	{
        PRINT("isGood returns true");
		return true;
	}

    PRINT("isGood returns false");
	return false;
}

static bool isRussian(int beg, int end)
{
    /*
    Russian alphabet in UTF-8
	(-48 -112)
	(-48 -111)
	(-48 -110)
	(-48 -109)
	(-48 -108)
	(-48 -107)
	(-48 -127)
	(-48 -106)
	(-48 -105)
	(-48 -104)
	(-48 -103)
	(-48 -102)
	(-48 -101)
	(-48 -100)
	(-48 -99)
	(-48 -98)
	(-48 -97)
	(-48 -96)
	(-48 -95)
	(-48 -94)
	(-48 -93)
	(-48 -92)
	(-48 -91)
	(-48 -90)
	(-48 -89)
	(-48 -88)
	(-48 -87)
	(-48 -86)
	(-48 -85)
	(-48 -84)
	(-48 -83)
	(-48 -82)
	(-48 -81)

	len: 66
	(-48 -80)
	(-48 -79)
	(-48 -78)
	(-48 -77)
	(-48 -76)
	(-48 -75)
	(-47 -111)
	(-48 -74)
	(-48 -73)
	(-48 -72)
	(-48 -71)
	(-48 -70)
	(-48 -69)
	(-48 -68)
	(-48 -67)
	(-48 -66)
	(-48 -65)
	(-47 -128)
	(-47 -127)
	(-47 -126)
	(-47 -125)
	(-47 -124)
	(-47 -123)
	(-47 -122)
	(-47 -121)
	(-47 -120)
	(-47 -119)
	(-47 -118)
	(-47 -117)
	(-47 -116)
	(-47 -115)
	(-47 -114)
	(-47 -113)

	len: 66

    */
    if ((beg == -48 && ((end >= -112 && end <= -65) || end == -127)) ||
        (beg == -47 && ((end >= -128 && end <= -113) || end == -111)))
    {
        PRINT("isRussian returns true");
        return true;
    }

    PRINT("isRussian returns false");
    return false;
}

static bool fix_name(std::string const& old_name, std::string& new_name)
{
    new_name.resize(0);
    bool changed = false;

    /*
    size_t count = 0;

    for (std::string::const_iterator it = old_name.begin(); it != old_name.end(); ++it)
    {
		if (!(count % 2))
		{
			printf("(%d ",(int)*it);
		}
		else
		{
			printf("%d)\n",(int)*it);
		}
        ++count;
    }
    */

    for (char const* p = old_name.c_str(); *p;)
    {
        if (isGood(*p))
        {
            new_name.append(1, *p++);
        }
        else
        {
            if (*p == ' ')
            {
                new_name.append(1, '_');
                ++p;
                changed = true;
            }
            else if (isRussian(*p, *(p + 1)))
            {
                new_name.append(1, *p++);
                new_name.append(1, *p++);
            }
            else
            {
                ++p;
                changed = true;
            }
        }
    }

    //printf("\ncount: %lu, old size: %lu, new size: %lu\n", count, old_name.size(), new_name.size());

    return changed;
}

static int move_file(char const* old_path, char const* new_path)
{
    printf("%s --> %s\n", old_path, new_path);
    if (rename(old_path, new_path) != 0)
    {
        fs::copy_file(old_path, new_path, fs::copy_options::overwrite_existing);
        fs::remove_all(old_path);
        //fprintf(stderr, "rename failed, error: %s\n",
          //  strerror(errno));
    }
    return 0;
}

int process_entry(char const* dir, char const* fname)
{
    std::string new_name;

    if (fix_name(fname, new_name))
    {
        std::string old_path(dir);
        old_path += "/";
        old_path += fname;

        std::string new_path(dir);
        new_path += "/";
        new_path += new_name;

        if (dry_run)
        {
            std::cout << old_path << " ===> " << new_path << '\n';
        }
        else
        {
            struct stat st;
            if (stat(new_path.c_str(), &st) == 0)
            {
                static bool all = false;
                std::string reply;

                if (!all)
                {
                    std::cout << "Renamed file exists: " << new_path << ", delete old: " << old_path << "? (y/n/a) " << std::flush;
                    std::cin >> reply;
                    if (reply == "a")
                    {
                        all = true;
                    }
                }

                if (all || reply == "y" || reply == "Y")
                {
                    fs::remove_all(old_path);
                }
            }
            else
            {
				move_file(old_path.c_str(), new_path.c_str());
			}
		}
    }

    return 0;
}

int process_dir(char const* dir_name)
{
    PRINT("process_dir: " << dir_name);

    DIR* dir = opendir(dir_name);

    if (!dir)
    {
        std::cerr << "process_dir failed: opendir failed, error: " << strerror(errno) << std::endl;
        return -1;
    }

    struct dirent* de;

    while ((de = readdir(dir)))
    {
        std::string full_path(dir_name);
        full_path += "/";
        full_path += de->d_name;

        struct stat st;
        if (stat(full_path.c_str(), &st) != 0)
        {
            std::cerr << "process_dir: stat failed, name: " << full_path << ", error: " << strerror(errno) << std::endl;
            continue;
        }

        PRINT("readdir got: de->d_type: " << (int)de->d_type << ", st.st_mode: " << st.st_mode << ", name: " << de->d_name);
        
        if (strcmp(de->d_name, ".") == 0 ||
            strcmp(de->d_name, "..") == 0)
        {
            continue;
        }

       
        if (st.st_mode & S_IFREG)
        //if (de->d_type == DT_REG)
        {
            process_entry(dir_name, de->d_name);
        }
        else if (st.st_mode & S_IFDIR)
        //else if (de->d_type == DT_DIR)
        {
            process_dir(full_path.c_str());

            process_entry(dir_name, de->d_name);
        }
    }

    closedir(dir);

    return 0;
}

// Invoke as this:
// recursive_ren [-n] dir1 [dir2...]

bool isHome(char const* dir)
{
    if (strcmp(dir, "~") == 0 ||
        strcmp(dir, "/home/andrew") == 0)
    {
        return true;
    }
    if (strcmp(dir, ".") == 0)
    {
        auto cwd = fs::current_path().string();
        //std::cout << cwd << std::endl;
        if (strcmp(cwd.c_str(), "/home/andrew") == 0)
        {
            return true;
        }
    }
    return false;
}

int main(int /*argc*/, char** argv)
{
    int start = 1;

    if (argv[1])
    {
        if (strcmp(argv[1], "-n") == 0)
        {
            dry_run = true;
            ++start;
        }
    }

    for (int i = start; argv[i]; ++i)
    {
        if (isHome(argv[i]))
        {
            std::cout << "Skipping home dir: " << argv[i] << std::endl;            
        }
        else
        {
            //std::cout << "Processing dir: " << argv[i] << std::endl;
            process_dir(argv[i]);
        }
    }

    return 0;
}

