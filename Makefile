#CHANGE THIS TO THE PATH OF YOUR PACS EXAMPLES FOLDER
PACS_ROOT = ../../pacs-examples/Examples


#NO NEED TO CHANGE ANYTHING BELOW THIS LINE

CXX ?= g++
CXXFLAGS ?= -std=c++20
EXEC = main

CPPFLAGS?= -Ofast -I./include -Wall -Wno-conversion-null -Wno-deprecated-declarations -I${PACS_ROOT}/include -I${PACS_ROOT}/include/muparserx -I${mkBoostInc}

LDFLAGS ?= -L${PACS_ROOT}/lib -L${mkBoostLib}
LIBS ?= -lmuparserx -lboost_filesystem -lboost_iostreams
OBJS := $(patsubst src/%.cpp,%.o,$(wildcard src/*.cpp))

MAKEFLAGS += -j2

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBS)

%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) *.o

distclean: clean
	$(RM) *~ $(EXEC) 
