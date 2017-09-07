#
# Rebuild the makefile for the assembler
#

LFMakeMaker >Makefile -v +g -cc="gcc -O3 -W -Wall -DDEBUG=1 -I../" --line=132 \
+f=Makefile \
-e=../bin/asm *.c
