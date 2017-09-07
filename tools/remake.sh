#
# Rebuild the makefile for tools
#

LFMakeMaker >Makefile -v +g -I../ -cc="g++ -O3 -W -Wall -DDEBUG=1 -I.. -L../sharplib" --line=132 +f=Makefile \
--opt="-lsharp" \
Dump2Bin.cxx -t=../bin/Dump2Bin \
Bin2Bas.cxx -t=../bin/Bin2Bas \
Relog.cxx -t=../bin/Relog \
CheckBin.cxx -t=../bin/CheckBin
