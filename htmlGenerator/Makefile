OS=$(shell uname)

EXECUTABLE = html_generator

ifeq ($(BUILD_TYPE),debug)
CFLAGS = -O0 -g -DDEBUG -D_DEBUG -D_GLIBCXX_DEBUG
else
CFLAGS = -O2
endif

CFLAGS += $(CXX_GLOB_FLAGS_WARN)

ifeq ($(OS),Linux)
CC = ccache g++
CXX = ccache g++
CXX_GLOB_FLAGS_WARN = -Werror -Wall -Wextra -Wno-unused-local-typedefs -Wno-empty-body
SYSTEM_LIBS=/usr/lib64
else
CC = ccache clang
CXX = ccache clang++
CXX_GLOB_FLAGS_WARN = -Werror -Wall -Wextra -Wno-empty-body
SYSTEM_LIBS=/usr/local/lib
endif

CPPFLAGS = -I/usr/local/include

CXXFLAGS = $(CFLAGS)

LDFLAGS += -L $(SYSTEM_LIBS) -lboost_filesystem -lboost_system -ljpeg

SRCS := $(wildcard *.cpp)

OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

DEPS := $(patsubst %.o,%.d,$(OBJS))

all: deploy

deploy: $(EXECUTABLE)

$(EXECUTABLE): $(DEPS) $(OBJS)
	$(CXX) $(LDFLAGS) -o $(EXECUTABLE) $(OBJS)

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,.*:,$*.o $@:,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

deploy:
	rsync -avu scripts/*.js /data/www/data
	rsync -avu makefiles/Make* ~/bin
	rsync -avu templates ~/bin
	rsync -avu $(EXECUTABLE) ~/bin

clean:
	rm -f $(OBJS) $(DEPS) *.stackdump *.d.*

distclean:
	rm -f $(OBJS) $(EXECUTABLE) $(DIST_EXE) $(DEPS) *.stackdump

explain:
	@echo "The following information represents my program:"
	@echo "Final executable name: $(EXECUTABLE)"
	@echo "Source files:     $(SRCS)"
	@echo "Object files:     $(OBJS)"
	@echo "Dependency files: $(DEPS)"

depend: $(DEPS)
	@echo "Dependencies are now up-to-date."

ifneq ($(MAKECMDGOALS),clean)
-include $(SRC:.cpp=.d)
endif

#-include $(DEPS)
