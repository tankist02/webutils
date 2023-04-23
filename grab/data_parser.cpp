#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <vector>
#include <string>
#include <cassert>


typedef char char_type;

static int const BUF_SIZE = 1024;
static char const SEPARATOR = '|';

#define SIZE_OF(arr) (sizeof(arr) / sizeof(*arr))


static char_type const* extract(char_type const* begin, char_type const* key, char_type* buf, size_t size)
{
    assert(begin);
    assert(key);
    assert(buf);
    assert(size > 0);

    char_type const* save = begin; // save in case the key is missing

    begin = strstr(begin, key);

    if( !begin )
    {
        *buf = '\0';
        return save;
    }

    begin += strlen(key);

    //char_type buf[BUF_SIZE];

    size_t i = 0;

    for( ; *begin && *begin != '<' && i < size - 1; ++i, ++begin )
    {
        //if( *begin != '\n' && *begin != '\r' ) //&& *begin != 0xc2 && *begin != 0x92 )
        if( *begin >= 32 && *begin <= 126 )
            buf[i] = (*begin == '|' ? '/' : *begin);
    }

    buf[i] = 0;

    return begin;
}

static int parse(char_type const* fname, FILE* fw, FILE* fc)
{
    FILE* fs = fopen(fname, "r");
    if( !fs )
    {
        fprintf(stderr, "Failed to open HTML file: %s, error: %s\n",
            fname, strerror(errno));

        return -1;
    }

    static char_type fbuf[128 * BUF_SIZE];

    /*size_t br = fread(fbuf, 1, SIZE_OF(fbuf), fs);
    if( br == SIZE_OF(fbuf) )
    {
        fprintf(stderr, "File: %s may be too big for buffer size: %lu\n", fname, SIZE_OF(fbuf));
        exit(-1);
    } */

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

    // Extract the name
    char_type const* KEY = "<TITLE>";
    char_type const* p1 = strstr(fbuf, KEY);
    if( !p1 )
    {
        fprintf(stderr, "File: %s doesn't contain substring %s\n", fname, KEY);
        exit(-1);
    }

    p1 += strlen(KEY);

    char_type const* p2 = p1;
    for( ; *p2 && *p2 != '\'' && *p2 != ' '; ++p2 )
        ;

    char_type name[BUF_SIZE];

    int len = std::min(long(SIZE_OF(name) - 1), long(p2 - p1));
    strncpy(name, p1, len);
    name[len] = 0;

    //static char_type const* FORMAT = "%10s";

    //printf("%-10s", buf);

    // Extract file key
    static int const KEY_LEN = 6;

    char_type key[KEY_LEN + 1];

    char_type const* p3 = strrchr(fname, '/');
    if( p3 )
        strncpy(key, p3 + 1, KEY_LEN);
    else
        strncpy(key, fname, KEY_LEN);

    key[KEY_LEN] = 0;

    fprintf(fw, "%s", key); // print file key first

    fprintf(fw, "%c%s", SEPARATOR, name); // print name

    p1 = strstr(p2, key);

    if( !p1 )
    {
        fprintf(stderr, "File: %s doesn't contain substring %s\n", fname, key);
        exit(-1);
    }

    p1 += KEY_LEN;

    char_type buf[BUF_SIZE];

    p1 = extract(p1, "Date of birth: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Age: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Height: </font>", buf, SIZE_OF(buf));
    int height = atoi(buf);
    fprintf(fw, "%c%d", SEPARATOR, height);
    p2 = strstr(buf, " ~ ");
    if( p2 )
        fprintf(fw, "%c%s", SEPARATOR, p2 + 3);
    else
        fprintf(fw, "%c", SEPARATOR);

    p1 = extract(p1, "Weight: </font>", buf, SIZE_OF(buf));
    int weight = atoi(buf);
    fprintf(fw, "%c%d", SEPARATOR, weight);
    p2 = strstr(buf, " ~ ");
    if( p2 )
    {
        weight = atoi(p2 + 3);
        fprintf(fw, "%c%d", SEPARATOR, weight);
    }
    else
        fprintf(fw, "%c", SEPARATOR);

    p1 = extract(p1, "Eye color: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Hair color: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Build body: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Education: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Profession: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Occupation: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Smokes: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Zodiac: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Marital status: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Number of children: </font>", buf, SIZE_OF(buf));
    int num = atoi(buf);
    fprintf(fw, "%c%d", SEPARATOR, num);

    for( int i = 0; i < num; ++i )
    {
        fprintf(fc, "%s", key);

        char_type lbuf[64];
        sprintf(lbuf, "Child %d Name: </font>", i + 1);
        p1 = extract(p1, lbuf, buf, SIZE_OF(buf));
        fprintf(fc, "%c%s", SEPARATOR, buf);

        sprintf(buf, "Child %d age: </font>", i + 1);
        p1 = extract(p1, buf, buf, SIZE_OF(buf));
        fprintf(fc, "%c%s", SEPARATOR, buf);

        sprintf(buf, "Child %d sex: </font>", i + 1);
        p1 = extract(p1, buf, buf, SIZE_OF(buf));
        fprintf(fc, "%c%c", SEPARATOR, *buf);

        fprintf(fc, "\n");
    }

    p1 = extract(p1, "In own words: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Seeks Partner: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Seeks Partner: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Her location: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%s", SEPARATOR, buf);

    p1 = extract(p1, "Introduction: </font>", buf, SIZE_OF(buf));
    fprintf(fw, "%c%d", SEPARATOR, atoi(buf));

    fprintf(fw, "\n");

    return 0;
}

int main(int argc, char** argv)
{
    char const* WOMEN_FILE = "women.data";
    FILE* fw = fopen(WOMEN_FILE, "w");
    if( !fw )
    {
        fprintf(stderr, "Failed to open file %s for writing, error: %s\n", WOMEN_FILE, strerror(errno));
        exit(-1);
    }

    char const* CHILDREN_FILE = "children.data";
    FILE* fc = fopen(CHILDREN_FILE, "w");
    if( !fc )
    {
        fprintf(stderr, "Failed to open file %s for writing, error: %s\n", CHILDREN_FILE, strerror(errno));
        exit(-1);
    }

    for( int i = 1; argv[i]; ++i )
    {
        parse(argv[i], fw, fc);
    }

    fclose(fw);
    fclose(fc);

    return 0;
}
