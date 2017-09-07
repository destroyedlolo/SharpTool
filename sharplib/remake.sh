#
# Rebuild the makefile for the library
#

LFMakeMaker >Makefile -v +g -I../ -cc="g++ -O3 -W -Wall -DDEBUG=1 -I.." --line=132 +f=Makefile \
-o *.cxx -b=libsharp.a
