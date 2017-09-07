# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 2.2 (Mar 26 2000 22:07:04) (c)LFSoft 1997

gotoall: all

# Warning : 'cctype' can't be located for this node.
hexa.o : hexa.cxx hexa.hxx Makefile 
	 g++ -O3 -W -Wall -DDEBUG=1 -I.. -c -o hexa.o hexa.cxx 

lectarg.o : lectarg.cxx lectarg.hxx ../lflib/lfchaine.hxx Makefile 
	 g++ -O3 -W -Wall -DDEBUG=1 -I.. -c -o lectarg.o lectarg.cxx 

libsharp.a : lectarg.o hexa.o Makefile 
	rm -f libsharp.a 
	ar rc libsharp.a lectarg.o hexa.o 
	ranlib libsharp.a 

all: libsharp.a 
