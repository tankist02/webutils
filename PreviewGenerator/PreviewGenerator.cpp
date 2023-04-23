#pragma warning( disable : 4786 4800 4251 )

#include <stdio.h>
#include <direct.h>
#include <string.h>
#include <stdlib.h>

#include <algorithm>
#include <functional>

#include "EnumFiles.h"
#include "ImageResizer.h"


#define POSTFIX "_preview"


static bool ends_with(const char* str, const char* substr)
{
  int len = strlen(str);
  int sublen = strlen(substr);

  if( sublen > len )
    return false;

  return memcmp(str + len - sublen, substr, sublen) == 0;
}

class PreviewGenerator
{
public:
  PreviewGenerator(int width, int height);

  int process();
  int printStats();

protected:
  int processFile(const char* in);

protected:
  int maxWidth;
  int maxHeight;

  int processed;
  int skipped;
  int failures;
};

PreviewGenerator::PreviewGenerator(int width, int height)
  : maxWidth(width)
  , maxHeight(height)
  , processed(0)
  , skipped(0)
  , failures(0)
{
}

int PreviewGenerator::process()
{
  for( EnumFiles ef("*.jp*g", EnumFiles::EF_FILES); ef(); ef.next() )
  {
    const char* imagename = ef.get().name;

    if( !(ends_with(imagename, ".jpg") || ends_with(imagename, ".jpeg")) 
       || ends_with(imagename, "_preview.jpg") || ends_with(imagename, "_preview.jpeg") )
      continue;

    processFile(imagename);
  }

  for( EnumFiles ed("*", EnumFiles::EF_SUBDIRS); ed(); ed.next() )
  {
    const char* subdir = ed.get().name;

    fprintf(stdout, "subdir: %s\n", subdir);
    
    // recurse into subdirs
    if( _chdir(subdir) != 0 )
    {
      fprintf(stderr, "Failed to changed to subdir: %s, error: %s\n", subdir, strerror(errno));
      continue;
    }
    process();
    _chdir("..");
  }

  return 0;
}

int PreviewGenerator::processFile(const char* in)
{
  static const int jquality = 80;
  int file_size;

  char out[_MAX_PATH];
  const char* p = strrchr(in, '.');
  if( p )
  {
    strncpy(out, in, p - in);
    out[p - in] = 0;
    strcat(out, POSTFIX);
    strcat(out, p);
  }
  else
  {
    strcpy(out, in);
    strcat(out, POSTFIX);
  }

  fprintf(stdout, "%s --> %s", in, out);

  int res = 0;

  int width = maxWidth;
  int height = maxHeight;
  ImageResize(in, out, &width, &height, &jquality, &file_size, &res);

  if( res == 0 )
    fprintf(stdout, ": success\n");
  else if( res == 1 )
    fprintf(stdout, ": skipped\n");
  else
    fprintf(stdout, ": failed\n");

  return 0;
}

int PreviewGenerator::printStats()
{
  return 0;
}

int main(int argc, const char** argv)
{
  if( argc != 3 )
  {
    fprintf(stderr, "Usage: %s <max_width> <max_height>\n", argv[0]);
    return 0;
  }

  int width = atoi(argv[1]);
  int height = atoi(argv[2]);

  PreviewGenerator pg(width, height);
  pg.process();
  pg.printStats();

  return 0;
}
