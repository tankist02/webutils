#pragma warning( disable : 4786 )

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <set>
#include <string>


class Process
{
public:
  int process(int argc, char** argv);
  int load(const char* fname);
  int save(const char* fname);

protected:
  int process(const char* fname);
  int processLine(const char* line);

  typedef std::set< std::string > Set;

  Set set_;
};

int main(int argc, char** argv)
{
  const char* db = "bad_ips.txt";

  Process proc;
  proc.load(db);
  proc.process(argc, argv);
  proc.save(db);

  return 0;
}

static bool isValidNumber(const char* num)
{
  int val = atoi(num);
  if( val < 0 || val > 255 )
    return false;
  return true;
}

static bool isValidIp(const char* ip)
{
  int dots = 0;
  char num[4];
  char* n = num;
  for( const char* p = ip; *p; ++p )
  {
    if( isdigit(*p) )
    {
      *n++ = *p;
    }
    else if( *p == '.' )
    {
      if( ++dots > 3 )
        return false;

      *n = 0;
      if( !isValidNumber(num) )
        return false;
      n = num;
    }
    else
      return false;
  }

  if( dots != 3 )
    return false;

  *n = 0;
  if( !isValidNumber(num) )
    return false;

  return true;
}

int Process::process(int argc, char** argv)
{
  for( int i = 1; i < argc; ++i )
    process(argv[i]);

  return 0;
}

int Process::load(const char* fname)
{
  FILE* fin = fopen(fname, "r");
  if( !fin )
  {
    fprintf(stderr, "Process::load: failed to open file for reading: %s, error: %s\n"
      , fname, strerror(errno));

    return -1;
  }

  char line[2048];

  while( fgets(line, sizeof(line), fin) )
    if( isValidIp(line) )
      set_.insert(line);

  fclose(fin);

  return set_.size();
}

int Process::save(const char* fname)
{
  FILE* fout = (fname && *fname && *fname != '-' ? fopen(fname, "w") : stdout);
  if( !fout )
  {
    fprintf(stderr, "Process::save: failed to open file for writing: %s, error: %s\n"
      , fname, strerror(errno));

    return -1;
  }

  for( Set::const_iterator it = set_.begin(); it != set_.end(); ++it )
  {
    if( fputs(it->data(), fout) == EOF || putc('\n', fout) == EOF )
    {
      fclose(fout);
      return -1;
    }
  }

  fclose(fout);

  return set_.size();
}

int Process::process(const char* fname)
{
  FILE* fin = fopen(fname, "r");
  if( !fin )
  {
    fprintf(stderr, "Process::process: failed to open file for reading: %s, error: %s\n"
      , fname, strerror(errno));

    return -1;
  }

  char line[2048];

  while( fgets(line, sizeof(line), fin) )
    processLine(line);

  fclose(fin);

  return 0;
}

int Process::processLine(const char* line)
{
  // extract ip
  char ip[16];
  *ip = 0;
  char* i = ip;

  const char* p = line;
  for( ; p && *p && !isspace(*p) && (i - ip) < sizeof(ip); ++p )
    *i++ = *p;

  *i = 0;

  if( !isValidIp(ip) )
    return -1;

  // extract HTTP status code (second from end)
  p = line + strlen(line) - 1;
  for( ; p > line && isspace(*p); --p ) // skip whitespace at the end
    ;

  for( ; p > line && !isspace(*p); --p ) // skip first word from the end
    ;

  for( ; p > line && isspace(*p); --p ) // skip whitespace before the last word
    ;

  for( ; p > line && !isspace(*p); --p ) // skip second word from the end
    ;
  ++p; // position at the start of the second word

  char code[4];
  i = code;
  for( ; p && *p && !isspace(*p) && (i - code) < sizeof(code); ++p )
    *i++ = *p;
  *i = 0;

  int val = atoi(code);
  if( val >= 400 && val < 600 )
    set_.insert(ip);

  return 0;
}
