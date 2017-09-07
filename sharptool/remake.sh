#
#	Rebuild the makefile of sharptool
#

# Please note : on some system (Solaris ...), it's needed to link w/ librt
# where nanosleep is ...
# -L/usr/X11R6/lib is needed on some systeme (Linux) where Xm in not in the
# standard search path for library. The directory should be changed for your system.

LFMakeMaker >Makefile -v +g -I../ -cc="g++ -O3 -W -Wall -DDEBUG=1 -I.. -L../sharplib" --line=132 \
+f=Makefile +f=../sharplib/libsharp.a \
--opt="-L/usr/X11R6/lib -lsharp -lXm -lXt -lX11 -lrt" \
-o *.cxx -t=../bin/sharptool
