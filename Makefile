#CHANGE THIS TO THE PATH OF YOUR PACS EXAMPLES FOLDER
# If you set the environmental variable PACS_ROOT in the .bashrc file
# simplifies thing. In my makefile it points directly to the Examples/ forlder
#PACS_ROOT = ../../pacs-examples
PP=$(PACS_ROOT)/..

#NO NEED TO CHANGE ANYTHING BELOW THIS LINE

CXX ?= g++
CXXFLAGS ?= -std=c++20
EXEC = main

CPPFLAGS?= -Ofast -I./include -Wall -Wno-conversion-null -Wno-deprecated-declarations -I${PP}/Examples/include -I${PP}/Examples/include/muparserx -I${mkBoostInc}

LDFLAGS ?= -L${PP}/Examples/lib -L${mkBoostLib}
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
