CXXFLAGS = -std=c++20
CPPFLAGS = -I ./include -I/usr/include -I/usr/include/x86_64-linux-gnu/ # I do not know why but the directory bits is in the last one
LDFLAGS = -L. -Wl,-rpath=${PWD}
LIBS = -lmuparserx

.phony= all clean distclean
.DEFAULT_GOAL = all
all: main

main: main.o
	$(CXX) $(LDFLAGS) main.o -o main $(LIBS)

main.o: main.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c main.cpp

clean:
	$(RM) *.o

distclean: clean
	$(RM) main
