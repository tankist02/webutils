EXECUTABLE = stats.exe

CPPFLAGS = -I../../utils/include
LDFLAGS =

LIBS = ../../utils/lib/libfileutils.a

CC = g++
CFLAGS = -Wall -O2

CXX = g++
CXXFLAGS = $(CFLAGS)

LINKCC = $(CXX) -L/usr/lib -L/usr/local/lib

SRCS := $(wildcard *.cc)

OBJS := $(patsubst %.cc,%.o,$(wildcard *.cc))

DEPS := $(patsubst %.o,%.d,$(OBJS))

all: $(EXECUTABLE)

$(EXECUTABLE): $(DEPS) $(OBJS)
	$(LINKCC) $(LDFLAGS) -o $(EXECUTABLE) $(OBJS) $(LIBS)
	strip $(EXECUTABLE)

%.d: %.cc
	$(CXX) -MM $(CPPFLAGS) $< > $@
	$(CXX) -MM $(CPPFLAGS) $< | sed s/\\.o/.d/ >> $@

clean:
	rm -f $(OBJS) $(DEPS) *.stackdump

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

-include $(DEPS)


