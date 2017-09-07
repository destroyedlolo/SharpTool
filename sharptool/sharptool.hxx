/*
 *	sharptool
 *	(c) L.Faillie	2002
 *
 *	For the moment, it's only a powerfull desassembleur for Sharp Pockets
 *
 *	Shared configuration
 *
 *	09/04/2002	Creation
 *	------------- V 1.0 -----------
 *	19/10/2010	Add second bank of memory for 2500
 */

#ifndef SHARP_TOOL_HXX
#define SHARP_TOOL_HXX 1

#include "emul.hxx"

#include <lflib/lfchaine.hxx>
#include <cstdio>

#define VERSION "sharptool V0.2 "__DATE__" "__TIME__
#define CPR	"(c) L.Faillie	2002-10"

extern unsigned char memory[];	// external memory (to be changed for banked machines
extern unsigned char memoryB1[];	// Bank1	
extern lfchaine prompt;
extern lfchaine home;			// Home directory
extern FILE *ftrace;
extern bool verbose;
extern bool vcs;				// Verifying the checksum
extern bool trace;				// if set, print each instruction executed in GUI mode
extern bool ignLCDOff;			// Ignore when the LCD is turned of
extern int nldisp;				// number of lines to display
extern CPU *cpu;				// active CPU

extern void dump_aff_reg(unsigned char);
extern void filename(lfchaine &);

#endif
