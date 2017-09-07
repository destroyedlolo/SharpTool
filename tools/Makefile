# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 2.2 (Mar 26 2000 22:07:04) (c)LFSoft 1997

gotoall: all

# Warning : 'cstring' can't be located for this node.
# Warning : 'cstdio' can't be located for this node.
# Warning : 'cstdlib' can't be located for this node.
Bin2Bas.o : Bin2Bas.cxx ../sharplib/hexa.hxx Makefile 
	 g++ -O3 -W -Wall -DDEBUG=1 -I.. -L../sharplib -c -o Bin2Bas.o Bin2Bas.cxx 

../bin/Bin2Bas : Bin2Bas.o Makefile 
	 g++ -O3 -W -Wall -DDEBUG=1 -I.. -L../sharplib -o ../bin/Bin2Bas Bin2Bas.o -lsharp 

# Warning : 'cstring' can't be located for this node.
# Warning : 'cstdio' can't be located for this node.
# Warning : 'cstdlib' can't be located for this node.
Relog.o : Relog.cxx ../sharplib/hexa.hxx Makefile 
	 g++ -O3 -W -Wall -DDEBUG=1 -I.. -L../sharplib -c -o Relog.o Relog.cxx 

../bin/Relog : Relog.o Makefile 
	 g++ -O3 -W -Wall -DDEBUG=1 -I.. -L../sharplib -o ../bin/Relog Relog.o -lsharp 

# Warning : 'cstring' can't be located for this node.
# Warning : 'cstdio' can't be located for this node.
# Warning : 'cstdlib' can't be located for this node.
CheckBin.o : CheckBin.cxx ../sharplib/hexa.hxx Makefile 
	 g++ -O3 -W -Wall -DDEBUG=1 -I.. -L../sharplib -c -o CheckBin.o CheckBin.cxx 

../bin/CheckBin : CheckBin.o Makefile 
	 g++ -O3 -W -Wall -DDEBUG=1 -I.. -L../sharplib -o ../bin/CheckBin CheckBin.o -lsharp 

Dump2Bin.o : Dump2Bin.cxx ../sharplib/hexa.hxx Makefile 
	 g++ -O3 -W -Wall -DDEBUG=1 -I.. -L../sharplib -c -o Dump2Bin.o Dump2Bin.cxx 

../bin/Dump2Bin : Dump2Bin.o Makefile 
	 g++ -O3 -W -Wall -DDEBUG=1 -I.. -L../sharplib -o ../bin/Dump2Bin Dump2Bin.o -lsharp 

all: ../bin/CheckBin ../bin/Relog ../bin/Bin2Bas ../bin/Dump2Bin
