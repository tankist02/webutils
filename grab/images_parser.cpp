#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <set>
#include <string>


#define SIZE_OF(arr) (sizeof(arr) / sizeof(*arr))


// Finds all URL(s) containing JPG images.
static int findImages(char const* fname)
{
    FILE* fs = fopen(fname, "r");
    if( !fs )
    {
        fprintf(stderr, "Failed to open HTML file: %s, error: %s\n",
            fname, strerror(errno));

        return -1;
    }

    static char fbuf[128 * 1024];

    size_t count = 0;

    int ch = 0;

    while( (ch = getc(fs)) != EOF && count < sizeof(fbuf) )
    {
        if( isprint(ch) )
            fbuf[count++] = ch;
    }

    if( count == SIZE_OF(fbuf) )
    {
        fprintf(stderr, "File: %s may be too big for buffer size: %lu\n", fname, SIZE_OF(fbuf));
        exit(-1);
    }

    fbuf[count] = 0;

    fclose(fs);

    static char const* JPEG_KEY = ".jpg\"";
    size_t klen = strlen(JPEG_KEY);

    char const* p1 = fbuf;

    while( true )
    {
        p1 = strstr(p1, JPEG_KEY);

        if( !p1 )
            break;

        char const* p2 = p1 - 1;
        while( *p2 != '"' && p2 > fbuf )
            --p2;

        if( p2 == fbuf )
        {
            fprintf(stderr, "File: %s doesn't contain symbol '\"' before %s", fname, JPEG_KEY);
            break;
        }

        p1 += klen;
        for( char const* p = p2; p != p1; ++p )
            putchar(*p);

        putchar('\n');


    }

    return 0;
}

int main(int argc, char** argv)
{
    for( int i = 1; argv[i]; ++i )
    {
        findImages(argv[i]);
    }

    return 0;
}
