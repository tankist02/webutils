#include <cstdio>
#include <cstring>
#include <sys/stat.h>


int main(int argc, char** argv)
{
    for( int i = 1; i < argc; ++i )
    {
        static int const BUF_SIZE = 3;

        char buf[3];

        strncpy(buf, argv[i], 2);
        buf[BUF_SIZE - 1] = 0;

        mkdir(buf, 0777);

        move(argv[i], buf);
    }

    return 0;
}
