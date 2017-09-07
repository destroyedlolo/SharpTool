#!/usr/bin/ksh
# Rebuild all makefile
#

for d in tools sharplib sharptool asm
do
	(cd $d && remake.sh)
done
