CXXFLAGS = -std=c++20
CPPFLAGS = -I ./include -I/usr/include -I/usr/include/x86_64-linux-gnu/ # I do not know why but the directory bits is in the last one
LDFLAGS = -L. -Wl,-rpath=${PWD}
LIBS = -lmuparserx

.phony= all clean distclean

.DEFAULT_GOAL = all

all: main

main: main.o my_parser.o
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBS)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) *.o

distclean: clean
	$(RM) main
