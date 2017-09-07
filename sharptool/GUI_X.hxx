/*
 * GUI_X.hxx
 *
 *	Stuff used by a GUI under X windows
 *
 *	06/11/2002	Creation
 */
#ifndef GUI_X_HXX
#define GUI_X_HXX

#ifdef __unix__

#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/ShellEP.h>
#include <Xm/DrawingA.h>

#define USE_X

	// Copy of parameters to intialise X
extern int argc;
extern char **argv;

	// X stuffs
extern Display *dsp;
extern Widget top_wnd, draw;

	// Misc
extern bool norefresh;	// If set, don't refresh the display 
						// (usefull for example to refresh to whole screen w/o
						// submiting X update for each screen octet).

extern void clean_X();
extern void gui_init(short int , short int, const char *);
// extern bool loadskin(const char *);
#endif

#endif
