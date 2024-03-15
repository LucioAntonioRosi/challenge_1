#You have to change the PACS_ROOT variable to the path of the PACS in your system
PACS_ROOT = ../../pacs-examples

CXX      ?= g++
CXXFLAGS ?= -std=c++20
EXEC = main

CPPFLAGS?= -O3 -I./include -Wall -Wno-conversion-null -Wno-deprecated-declarations -I${PACS_ROOT}/Examples/include -I${PACS_ROOT}/Examples/include/muparserx
LDFLAGS ?= -L${PACS_ROOT}/Examples/lib
LIBS ?= -lmuparserx
OBJS := $(patsubst src/%.cpp,%.o,$(wildcard src/*.cpp))

MAKEFLAGS += -j2

all: $(EXEC)

main: $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBS)

%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) *.o

distclean: clean
	$(RM) *~ $(EXEC)
