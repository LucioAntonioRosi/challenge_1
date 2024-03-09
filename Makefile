#make:
#	g++ -I /usr/local/include -std=c++20 -o main main.cpp -L.  -lmuparserx
CXXFLAGS = -std=c++20
CPPFLAGS = -DNDEBUG -I ./include
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
