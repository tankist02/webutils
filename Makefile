#CXXFLAGS+=-I /usr/local/include -Wall -Wextra -Wpedantic -Werror -Wfloat-equal -Wno-unused-parameter -std=c++20 -O0 -g3
CXXFLAGS=-I /usr/local/include -Wall -Wextra -Wpedantic -Werror -Wfloat-equal -Wno-unused-parameter -std=c++20 -O3

LDFLAGS+=-L /usr/local/lib -lboost_filesystem -lboost_system

clean:
	find . -type f -perm +111 | xargs rm -f
	rm -f *.o

