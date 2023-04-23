#include <iostream>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <sys/param.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <functional>

#include "EnumFiles.h"

//#define TRACE(s) fprintf(stdout, s)
//#define TRACE1(s, a1) fprintf(stdout, s, a1)
#define TRACE(s)
#define TRACE1(s, a1)

#define IP_HITS_THRESHOLD 2
#define IMAGE_HITS_THRESHOLD 1

static std::string& toLower(std::string& str)
{
  for( std::string::iterator it = str.begin(); it != str.end(); ++it )
    *it = tolower(*it);

  return str;
}

static std::string trim(const std::string& str)
{
  const char* p1, *p2;

  for( p1 = str.c_str(); *p1 && isspace(*p1); ++p1 )
    ;

  for( p2 = str.c_str() + str.length() - 1; p2 >= p1 && isspace(*p2); --p2 )
    ;

  return std::string(p1, p2 - p1 + 1);
}

static std::string& addSlash(std::string& s)
{
  if( s.empty() )
    s = "/";
  else
  {
    std::replace_if(s.begin(), s.end(), std::bind2nd(std::equal_to< char >(), '\\'), '/');
    if( s[s.length() - 1] != '/' )
      s += '/';
  }
  return s;
}

class Info
{
public:
  Info();
  Info(const Info& i);
  Info& operator=(const Info& i);

  int parseApache(const char* line);
  int parseIIS4(const char* line);
  int parseIIS5(const char* line);
  int print() const;

public:
  std::string ip;
  std::string dateTime;
  std::string request;
  int code;
  int size;

protected:
  Info& copy(const Info& i);
};

Info::Info()
{
  code = 0;
  size = 0;
}

Info::Info(const Info& i)
{
  copy(i);
}

Info& Info::operator=(const Info& i)
{
  return copy(i);
}

Info& Info::copy(const Info& i)
{
  ip = i.ip;
  dateTime = i.dateTime;
  request = i.request;
  code = i.code;
  size = i.size;
  return *this;
}

int Info::parseApache(const char* line)
{
  //fprintf(stdout, "%s<br>\n", line);

  //int res = sscanf(line, "%s %s %s [%s] \"%s\" %d %d", ip, dummy1, dummy2, dateTime, request, &code, &size);

  int count = 1;
  bool word = false;
  bool bracket = false;
  bool quotes = false;

  std::string token;
  token.reserve(256);

  for( const char* p = line; p && *p; ++p )
  {
    TRACE1("%c: ", *p);

    if( '[' == *p )
    {
      TRACE("bracket starts\n");
      bracket = true;
      token.resize(0);
    }
    else if( ']' == *p )
    {
      TRACE("bracket ends\n");
      bracket = false;
      dateTime = token;
    }
    else if( '"' == *p )
    {
      if( !quotes )
      {
        TRACE("quotes starts\n");
        quotes = true;
        token.resize(0);
      }
      else
      {
        TRACE("quotes ends\n");
        quotes = false;

        /*
        const char* p1 = strchr(token.c_str(), ' ');
        const char* p2 = strrchr(token.c_str(), ' ');
        if( p1 && p2 && p2 - p1 > 1 )
        {
          request = std::string(p1 + 1, p2 - p1 - 1);
          //toLower(request);
        }
        */

        std::string::size_type p1 = token.find(' ');
        if( p1 != std::string::npos )
        {
          std::string::size_type p2 = token.rfind(' ');
          if( p2 != std::string::npos && p2 - p1 > 1 )
            request = token.substr(p1 + 1, p2 - p1 - 1);
        }

        //fprintf(stdout, "token: %s, request: %s\n", token.c_str(), request.c_str());
      }
    }
    else if( isspace(*p) )
    {
      if( bracket )
      {
        TRACE("inside bracket\n");
        token += *p;
      }
      else if( quotes )
      {
        TRACE("inside quotes\n");
        token += *p;
      }
      else
      {
        if( word )
        {
          TRACE("word ends\n");
          word = false;
          switch( count )
          {
            case 1:
              ip = token;
              break;

            case 4:
              code = atoi(token.c_str());
              break;

            case 5:
              size = atoi(token.c_str());
              ++count;
              //fprintf(stdout, "size: %d, count: %d, token: %s<br>\n",
                //size, count, token.c_str());
              return count;

            default:
              ;
              //fprintf(stderr, "invalid count: %d, token: %s\n", count, token.c_str());
          }
          ++count;
        }
      }
    }
    else // not [, ], " or white space
    {
      if( bracket )
      {
        TRACE("inside bracket\n");
        token += *p;
      }
      else if( quotes )
      {
        TRACE("inside quotes\n");
        token += *p;
      }
      else if( word )
      {
        TRACE("inside word\n");
        token += *p;
      }
      else
      {
        TRACE("word starts\n");
        word = true;
        token.resize(0);
        token += *p;
      }
    }
  }

  size = atoi(token.c_str());
  ++count;

  fprintf(stdout, "size: %d, count: %d<br>\n", size, count);
  return count;
}

int Info::parseIIS4(const char* line)
{
  //fprintf(stdout, "%s\n", line);

  //int res = sscanf(line, "%s %s %s [%s] \"%s\" %d %d", ip, dummy1, dummy2, dateTime, request, &code, &size);
  //63.205.81.196, -, 9/14/00, 13:12:41, W3SVC3, C50023-B, 24.15.211.40, 661, 336, 8835, 200, 0, GET, /Las_Vegas_November_1999/night/Bellagio_front_and_lake_preview.jpg, -,

  int count = 1;
  bool word = false;

  std::string token;
  token.reserve(256);

  for( const char* p = line; p && *p; ++p )
  {
    TRACE1("%c: ", *p);

    if( isspace(*p) || ',' == *p )
    {
      if( word )
      {
        TRACE("word ends\n");
        word = false;
        switch( count )
        {
          case 1: ip = token; break;
          case 11: code = atoi(token.c_str()); break;
          case 14: request = toLower(token); return count; //break;
          default:
            ;
            //fprintf(stderr, "invalid count: %d, token: %s\n", count, token.c_str());
        }
        ++count;
      }
    }
    else // not white space
    {
      if( word )
      {
        TRACE("inside word\n");
        token += *p;
      }
      else
      {
        TRACE("word starts\n");
        word = true;
        token.resize(0);
        token += *p;
      }
    }
  }

  return count;
}

int Info::parseIIS5(const char* line)
{
  if( '#' == *line )
    return 0;
  //fprintf(stdout, "%s\n", line);

  //int res = sscanf(line, "%s %s %s [%s] \"%s\" %d %d", ip, dummy1, dummy2, dateTime, request, &code, &size);
  //2001-07-13 23:56:41 65.192.247.142 - 24.15.211.40 80 GET /Family/Mishka/Mishka.html - 200 Mozilla/4.0+(compatible;+MSIE+5.01;+Windows+NT+5.0)

  int count = 1;
  bool word = false;

  std::string token;
  token.reserve(256);

  for( const char* p = line; p && *p; ++p )
  {
    TRACE1("%c: ", *p);

    if( isspace(*p) )
    {
      if( word )
      {
        TRACE("word ends\n");
        word = false;
        switch( count )
        {
          case 3: ip = token; break;
          case 8: request = toLower(token); break;
          case 10: code = atoi(token.c_str()); return count; //break;
          default:
            ;
            //fprintf(stderr, "invalid count: %d, token: %s\n", count, token.c_str());
        }
        ++count;
      }
    }
    else // not white space
    {
      if( word )
      {
        TRACE("inside word\n");
        token += *p;
      }
      else
      {
        TRACE("word starts\n");
        word = true;
        token.resize(0);
        token += *p;
      }
    }
  }

  return count;
}

int Info::print() const
{
  fprintf(stdout, " ip: %s\n dateTime: %s\n request: %s\n code: %d\n size: %d\n"
    , ip.c_str(), dateTime.c_str(), request.c_str(), code, size);
  return 0;
}

class InfoArray
{
public:
  int doImages();
  int doIps();
  int config(const char* fname);

protected:
  enum Type
  {
    APACHE,
    IIS4,
    IIS5,
    UNKNOWN
  };
  int findFiles(const std::string& path);

  int countRequests(Type type, const char* fname);
  int print() const;
  int printFiles() const;

  int count(Type type);
  Type getType(const char* fname);
  bool parse(Type type, const char* line, Info& info);

protected:
  typedef std::map< std::string, int > Map;
  Map map;

  //typedef std::pair< std::string, int > Entry;
  typedef std::pair< const char*, int > Entry;
  typedef std::vector< Entry > Entries;

  class ImageComp
  {
  public:
    bool operator()(const Entry& e1, const Entry& e2)
    {
      return e1.second > e2.second; // descending order
    }
  };

  //static const char* log_paths[];
  typedef std::vector< std::string > LogPaths;
  LogPaths log_paths;

  std::string docRoot;

  int apache_count;
  int iis4_count;
  int iis5_count;

  int ipHitsThreshold;
  int imageHitsThreshold;

  typedef std::set< std::string > Ips;
  Ips ignoreIps;
};

int InfoArray::config(const char* fname)
{
  ipHitsThreshold = IP_HITS_THRESHOLD;
  imageHitsThreshold = IMAGE_HITS_THRESHOLD;

  ignoreIps.clear();

  //char buf[MAXPATHLEN];
  //fprintf(stdout, "%s<br>\n", getcwd(buf, sizeof(buf)));

  if( !fname || !*fname )
    fname = "./stats.conf";

  FILE* fin = fopen(fname, "r");
  if( !fin )
  {
    fprintf(stderr, "failed to open file: %s for reading, error: %s\n",
      fname, strerror(errno));
    return -1;
  }

  char line[1024];
  while( fgets(line, sizeof(line), fin) )
  {
    if( *line == 0 || *line == '#' )
      continue;

    const char* p = strchr(line, '=');
    if( !p )
      continue;

    std::string name = trim(std::string(line, p - line));
    std::string value = trim(std::string(p + 1));

    if( 0 == strcmp(name.c_str(), "log_path") )
      log_paths.push_back(value); //addSlash(value));
    else if( 0 == strcmp(name.c_str(), "ignore_ip") )
      ignoreIps.insert(value);
    else if( 0 == strcmp(name.c_str(), "ip_hits_threshold") )
      ipHitsThreshold = atoi(value.c_str());
    else if( 0 == strcmp(name.c_str(), "image_hits_threshold") )
      imageHitsThreshold = atoi(value.c_str());
  }

  //fprintf(stdout, "log paths:<br>\n");
  for( LogPaths::const_iterator lit = log_paths.begin(); lit != log_paths.end(); ++lit )
    //fprintf(stdout, "'%s'<br>\n", lit->data());

  if( !ignoreIps.empty() )
  {
    fprintf(stdout, "ignore ips:<br>\n");
    for( Ips::const_iterator it = ignoreIps.begin(); it != ignoreIps.end(); ++it )
      fprintf(stdout, "%s<br>\n", it->data());
  }

  fprintf(stdout, "ip hits threshold: %d, image hits threshold: %d<br>\n", ipHitsThreshold, imageHitsThreshold);

  fclose(fin);

  return 0;
}

int InfoArray::count(Type type)
{
  int count = 0;
  switch( type )
  {
    case APACHE:
      ++apache_count;
      break;

    case IIS4:
      ++iis4_count;
      break;

    case IIS5:
      ++iis5_count;
      break;

    default:
      ++count;
      break;
  }
  return count;
}

InfoArray::Type InfoArray::getType(const char* fname)
{
  const char* p = strrchr(fname, '/');
  if( !p )
    p = fname;
  else
    ++p; // to skip '/'

  //fprintf(stdout, "p: %s<br>\n", p);

  if( 0 == strncmp(p, "access", 6) )
    return APACHE;

  if( 0 == strncmp(p, "in", 2) )
    return IIS4;

  if( 0 == strncmp(p, "ex", 2) )
    return IIS5;

  return UNKNOWN;
}

bool InfoArray::parse(Type type, const char* line, Info& info)
{
  bool success = false;

  switch( type )
  {
    case APACHE:
      success = (6 == info.parseApache(line) );
      break;

    case IIS4:
      success = (14 == info.parseIIS4(line));
      break;

    case IIS5:
      success = (10 == info.parseIIS5(line));
      break;

    default:
      return false;
  }
  return success;
}

int InfoArray::doIps()
{
  fprintf(stdout, "doIps invoked<br>\n");

  //const std::string log_mask = "*.log";

  int success_count = 0;
  int fail_count = 0;

  Map map;

  apache_count = 0;
  iis4_count = 0;
  iis5_count = 0;

  for( LogPaths::const_iterator it = log_paths.begin(); it != log_paths.end(); ++it )
  {
    const std::string& log_path = *it;

    //fprintf(stdout, "log_path: '%s'<br>\n", log_path.c_str());

    for( EnumFiles ef_files(log_path.c_str(), EnumFiles::EF_FILES); ef_files.next(); )
    {
      std::string fname = ef_files.getFullName();

      //fprintf(stdout, "fname: %s<br>\n", fname.c_str());

      Type type = getType(fname.c_str());

      if( UNKNOWN == type )
        continue;

      //fprintf(stdout, "type: %d<br>\n", type);

      FILE* fin = fopen(fname.c_str(), "r");
      if( !fin )
      {
        fprintf(stderr, "failed to open file: %s for reading, error: %s\n", fname.c_str(), strerror(errno));
        continue;
      }

      Info info;
      char line[10240];
      while( fgets(line, sizeof(line), fin) )
      {
        bool success = parse(type, line, info);
        //fprintf(stdout, "success: %d, info.code: %d<br>\n",
          //success, info.code);

        if( success )
        {
          if( 200 <= info.code && 400 > info.code )
          {
            ++success_count;

            if( ignoreIps.find(info.ip) == ignoreIps.end() )
            {
              Map::iterator it = map.find(info.ip);
              if( map.end() == it )
                map.insert(Map::value_type(info.ip, 1));
              else
                it->second++;
            }
            count(type);
          }
          else
            ++fail_count;
        }
      }

      fclose(fin);
    }
  }

  fprintf(stdout, "apache_count: %d, iis4_count: %d, iis5_count: %d<br>\n", apache_count, iis4_count, iis5_count);

  fprintf(stdout, "requests: successes: %d, others: %d (including hacking attempts)<br>\n", success_count, fail_count);

  Entries entries;
  entries.reserve(map.size());

  for( Map::const_iterator mit = map.begin(); mit != map.end(); ++mit )
  {
    if( mit->second >= ipHitsThreshold )
      entries.push_back(Entry(mit->first.c_str(), mit->second));
  }

  std::sort(entries.begin(), entries.end(), ImageComp());

  fprintf(stdout, "total ips: %d<br>\n", entries.size());

  fprintf(stdout, "<p><table border=1 cellpadding=15>\n");
  //fprintf(stdout, "<p><table border=1 cellspacing=0 cellpadding=5 width=\"90%%\">\n");

  fprintf(stdout, "<tr><td><h3>Ip</h3></td><td><h3>Hits</h3></td></tr>\n");

  for( Entries::const_iterator eit = entries.begin(); eit != entries.end(); ++eit )
  {
    fprintf(stdout, "<tr>\n");

    fprintf(stdout, "<td>%s</td> <td>%d</td>\n", eit->first, eit->second);
    fprintf(stdout, "</tr>\n");
  }

  fprintf(stdout, "</table>\n");

  return 0;
}

int InfoArray::doImages()
{
  docRoot = getenv("DOCUMENT_ROOT");

  const char* referrer = getenv("HTTP_REFERER");

  std::string dir;

  const char* p = strstr(referrer, "//");
  p = strchr(p + 2, '/');
  if( p )
  {
    //fprintf(stdout, "p: %s<br>\n", p);
    const char* rp = strrchr(p + 1, '/');
    //fprintf(stdout, "rp: %s<br>\n", rp);
    if( rp && rp - p + 1 > 0 )
      dir = std::string(p, rp - p + 1);
    else
      dir = p;
  }

  addSlash(dir);

  std::string path = docRoot + dir;

  //fprintf(stdout, "<p>path: %s<br>\n", path.c_str());
  //fprintf(stdout, "docRoot: %s, referrer: %s<br></body></html>\n", docRoot.c_str(), referrer);
  // exit(0);

  //fprintf(stdout, "files: %d<br>\n", findFiles(path));

  findFiles(path);

  //const std::string log_mask = "*.log";

  apache_count = 0;
  iis4_count = 0;
  iis5_count = 0;

  for( LogPaths::const_iterator it = log_paths.begin(); it != log_paths.end(); ++it )
  {
    const std::string& log_path = *it;
    //fprintf(stdout, "log_path: %s<br>\n", log_path.c_str());

    for( EnumFiles ef_files(log_path.c_str(), EnumFiles::EF_FILES); ef_files.next(); )
    {
      std::string fname = ef_files.getFullName();

      Type type = getType(fname.c_str());

      //fprintf(stdout, "fname: %s, type: %d<br>\n", fname.c_str(), type);

      if( UNKNOWN == type )
        continue;

      countRequests(type, fname.c_str());
    }
  }
  //printFiles();

  Entries entries;
  entries.reserve(map.size());

  for( Map::const_iterator mit = map.begin(); mit != map.end(); ++mit )
    if( mit->second >= imageHitsThreshold )
      entries.push_back(Entry(mit->first.c_str(), mit->second));

  std::sort(entries.begin(), entries.end(), ImageComp());

  fprintf(stdout, "apache_count: %d, iis4_count: %d, iis5_count: %d<br>\n", apache_count, iis4_count, iis5_count);
  fprintf(stdout, "images count: %d<br>\n", entries.size());

  fprintf(stdout, "<p><table border=1 cellpadding=15>\n");
  //fprintf(stdout, "<p><table border=1 cellspacing=0 cellpadding=5 width=\"90%%\">\n");

  fprintf(stdout, "<tr><td><h3>Hits</h3></td><td><h3>Path</h3></td><td><h3>Link</h3></td></tr>\n");

  for( Entries::const_iterator eit = entries.begin(); eit != entries.end(); ++eit )
  {
    fprintf(stdout, "<tr>\n");

    //<a href="crop0022.jpg"><img src="crop0022_preview.jpg" hspace=0 vspace=0 border=0><br></a>"
    const char* p = strrchr(eit->first, '.');
    std::string preview;
    if( p )
    {
      preview = std::string(eit->first, p - eit->first);
      preview += "_preview.jpg";
    }
    std::string link = std::string("<a href=\"") + eit->first + "\"><img src=\"" + preview + "\"></a>";

    fprintf(stdout, "<td>%d</td> <td>%s</td> <td>%s</td>\n", eit->second, eit->first, link.c_str());
    fprintf(stdout, "</tr>\n");
  }

  fprintf(stdout, "</table>\n");

  return 0;
}

int InfoArray::findFiles(const std::string& path)
{
  std::string path_mask = path + "*.jpg";

  //fprintf(stdout, "findFiles: path_mask: %s<br>\n", path_mask.c_str());

  for( EnumFiles ef_files(path_mask.c_str(), EnumFiles::EF_FILES); ef_files.next(); )
  {
    //fprintf(stdout, "%s\n", (path + ef_files.get().name).c_str());
    std::string fname = ef_files.getFullName();

    int len = fname.length();
    if( len < 12 || strcmp(fname.c_str() + len - 12, "_preview.jpg") != 0 )
    {
      std::string tmp = fname.substr(docRoot.length());
      //toLower(tmp);
      map.insert(Map::value_type(tmp, 0));
    }
  }

  std::string new_mask = path + "*";
  for( EnumFiles ef_subdirs(new_mask.c_str(), EnumFiles::EF_SUBDIRS); ef_subdirs.next(); )
  {
    std::string new_path = ef_subdirs.getFullName();
    new_path += "/";
    findFiles(new_path);
  }
  //printFiles();
  return map.size();
}

int InfoArray::countRequests(Type type, const char* fname)
{
  //fprintf(stdout, "countRequest: fname: %s<br>\n", fname);

  FILE* fin = fopen(fname, "r");
  if( !fin )
  {
    fprintf(stderr, "failed to open file: %s for reading, error: %s\n", fname, strerror(errno));
    return -1;
  }

  Info info;
  char line[10240];
  while( fgets(line, sizeof(line), fin) )
  {
    bool success = parse(type, line, info);
    //fprintf(stdout, "success: %d, info.code: %d, request: %s<br>\n",
      //success, info.code, info.request.c_str());

    if( success && 200 <= info.code && 400 > info.code )
    {
      if( ignoreIps.find(info.ip) == ignoreIps.end() )
      {
        Map::iterator it = map.find(info.request);
        if( it != map.end() )
        {
          it->second++;
          count(type);
          //fprintf(stdout, "found request: '%s'<br>\n", info.request.c_str());
        }
        else
        {
          //fprintf(stdout, "not found request: '%s'<br>\n", info.request.c_str());
        }
      }
      //vector.push_back(info);
    }
  }

  fclose(fin);
  return 0; //vector.size();
}

/*
int InfoArray::print() const
{
  for( Vector::const_iterator it = vector.begin(); it != vector.end(); ++it )
    it->print();

  return 0;
}
*/

int InfoArray::printFiles() const
{
  for( Map::const_iterator it = map.begin(); it != map.end(); ++it )
    fprintf(stdout, "%d: '%s'<br>\n", it->second, it->first.c_str());

  return 0;
}

class Html
{
public:
  Html()
  {
    fprintf(stdout, "Content-type: text/html\n\n");
    fprintf(stdout, "<html><body>\n");
  }

  ~Html()
  {
    fprintf(stdout, "</body></html>\n");
  }
};

int main(int argc, char* argv[])
{
  //const char* line = "2001-07-13 23:56:41 65.192.247.142 - 24.15.211.40 80 GET /Family/Mishka/Mishka.html - 200 Mozilla/4.0+(compatible;+MSIE+5.01;+Windows+NT+5.0)";
  //Info().parseIIS5(line);

  //const char* line = "63.205.81.196, -, 9/14/00, 13:12:41, W3SVC3, C50023-B, 24.15.211.40, 661, 336, 8835, 200, 0, GET, /Las_Vegas_November_1999/night/Bellagio_front_and_lake_preview.jpg, -,";
  //Info().parseIIS4(line);

  /*
  const char* str = "192.168.0.2 - - [01/Jun/2003:22:52:46 -0700] \"GET /California/Asilomar/crop0015.html HTTP/1.1\" 200 427 \"http://192.168.0.3:81/California/Asilomar/Asilomar.html\" \"Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.3) Gecko/20030314\"";
  Info info;
  info.parseApache(str);
  info.print();

  exit(0);
  */

  Html html;

  //for( const char** var = (const char**)_environ; *var; *var++ )
    //fprintf(stdout, "%s<br>\n", *var);

  if( !argv[1] )
  {
    fprintf(stdout, "Usage: %s &lt;ips | images&gt;\n", argv[0]);
    return 0;
  }

  /*
  //const char* str = "10.1.11.1 - - [04/May/2001:13:44:53 -0700] \"GET /web_site/lp/wine_tasting/f10_preview.jpg HTTP/1.0\" 200 4494";
  const char* str = "63.195.120.124 - - [11/Jan/2003:17:51:32 -0800] \"GET /Travel/Miami_2002/CocoWalk/crop0056.html HTTP/1.1\" 200 413";

  Info info;
  info.parseApache(str);
  info.print();
  */

  InfoArray arr;
  arr.config(NULL);

  if( 0 == strcmp(argv[1], "ips") )
    arr.doIps();
  else if( 0 == strcmp(argv[1], "images") )
    arr.doImages();
  else
    fprintf(stdout, "Invalid arg: %s\n", argv[1]);

  return 0;
}

