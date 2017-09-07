/*
 *	config.cxx
 *	(c) L.Faillie	2002
 *
 *	Configuration
 *
 *	10/04/2002	Creation
 *	------------- V 1.0 -----------
 *	19/10/2010	Add second bank of memory for 2500
 */
#include "sharptool.hxx"

#include <cassert>

lfchaine prompt(">");
unsigned char memory[65536];
unsigned char memoryB1[65536];

lfchaine home;

FILE *ftrace=NULL;
bool verbose=false;
bool vcs=true;			// checksum has to be verified
bool trace=false;		// if set, print each instruction executed in GUI mode
int nldisp;				// Number of lines to display
bool ignLCDOff=false;			// Ignore when the LCD is turned of

CPU *cpu=NULL;

void filename(lfchaine &ch){
	if(**ch == '~'){
		lfchaine t(*home);
		t += (*ch)+1;
		ch = t;
	}
}

