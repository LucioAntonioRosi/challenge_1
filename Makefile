CXXFLAGS = -std=c++20
CPPFLAGS = -I ./include -I/usr/include -I/usr/include/x86_64-linux-gnu/ # I am not able to include <nlohmann/json>
LDFLAGS = -L. -Wl,-rpath=${PWD}
LIBS = -lmuparserx

.PHONY: all clean distclean

all: main

main: main.o
	$(CXX) $(LDFLAGS) main.o -o main $(LIBS)

main.o: main.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c main.cpp

clean:
	$(RM) *.o

distclean: clean
	$(RM) main
