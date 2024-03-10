# challenge_1

#If you do not have the json library installed, copy the following command to install it:

sudo apt-get install nlohmann-json3-dev

## A PROBLEM THAT I ENCOUNTERED 
 When i try to add the Json header, it gives me the following error:

 /u/sw/toolchains/gcc-glibc/11.2.0/prefix/bin/g++ -I ./include -I /usr/include   -std=c++20 -c main.cpp
In file included from /u/sw/toolchains/gcc-glibc/11.2.0/prefix/include/c++/11.2.0/x86_64-mk-linux/bits/gthr-default.h:35,
                 from /u/sw/toolchains/gcc-glibc/11.2.0/prefix/include/c++/11.2.0/x86_64-mk-linux/bits/gthr.h:148,
                 from /u/sw/toolchains/gcc-glibc/11.2.0/prefix/include/c++/11.2.0/ext/atomicity.h:35,
                 from /u/sw/toolchains/gcc-glibc/11.2.0/prefix/include/c++/11.2.0/bits/ios_base.h:39,
                 from /u/sw/toolchains/gcc-glibc/11.2.0/prefix/include/c++/11.2.0/ios:42,
                 from /u/sw/toolchains/gcc-glibc/11.2.0/prefix/include/c++/11.2.0/ostream:38,
                 from /u/sw/toolchains/gcc-glibc/11.2.0/prefix/include/c++/11.2.0/iostream:39,
                 from main.cpp:2:
/usr/include/pthread.h:33:11: fatal error: bits/pthread_stack_min-dynamic.h: No such file or directory
   33 | # include <bits/pthread_stack_min-dynamic.h>
      |           ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make: *** [Makefile:14: main.o] Error 1

#Because /bits is not in /usr/include ... Is this a problem? Ask the professor...

## IMPLEMENT A HEADER FILE FOR THE GRADIENT