/*
 * gui_X_1350.cxx
 *
 *	GUI for PC-1350 under X windows
 *
 *
 *	06/11/2002	Creation
 */

#ifdef __unix__

#include "PC-1350.hxx"
#include "GUI_X.hxx"
#include "sharptool.hxx"

#include <cstdio>
#include <X11/keysym.h>

#define LCD_WIDTH	(150 + 3 + 17 + 1) * 2
#define LCD_HEIGHT	(32 + 1) * 2

void pc1350K::gui(){
/* Size of the screen :
 * - margin : 5 px
 * - width : 150 + 2*5 + 17 ( screen + 3 margines + size of SHIFT )
 *	so the Sharp video starts at px 27.
 * - height : 32 + 2*5
 *
 *	All pixel are magnified by 2
 */
 
	gui_init( LCD_WIDTH, LCD_HEIGHT, "Sharptool PC-1350");	// Init all X stuff
}

void pc1350K::LCDOff(){
	if(!have_gui())
		return;

	XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  WhitePixel(dsp, DefaultScreen(dsp)) );

	XFillRectangle(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 0,0, LCD_WIDTH, LCD_HEIGHT);

	if(!norefresh)
		XFlush(dsp);
}

unsigned char pc1350K::customkey(long int code){
	switch(code){
		// On a qwerty keyboard, these key doesn't exist
	case	'\\'	: return(':');
	case	']'	: return(')');
	case	'['	: return('(');

	case	XK_Insert	: return(KEY_INS);
	case	XK_BackSpace :
	case	XK_Delete	: return(KEY_DEL);
	case	XK_Shift_L	: return(KEY_SHIFT);
	}
	return(0);
}

void pc1350K::updatevideo(unsigned short int adr){
	if(!have_gui())
		return;

	if(!(OutC() & OUTC_LCD) && !ignLCDOff)	// don't do anything if the display is off
		return;

	unsigned char val;
	if((adr & 0xf000) != 0x7000 || adr >= 0x7a00)	// It's not inside video RAM
		return;

 	adr &= 0xfeff;	// Remove memory overlap

	if(adr == 0x783C){
		if((val = pread(0x783C)) & 1){	// Shift
			XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  		BlackPixel(dsp, DefaultScreen(dsp)) );

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 6,39,7,39);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 5,40,7,42);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 5,43,6,43);

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 9,39,9,43);
			XDrawPoint(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 10,41);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 11,39,11,43);

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 13,39,13,43);

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 15,39,15,43);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 16,39,17,39);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 16,41,17,41);

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 19,39,21,39);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 20,40,20,43);
		} else {
			XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  		WhitePixel(dsp, DefaultScreen(dsp)) );
			XFillRectangle(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 5,39,20,5);
		}
		
		if(val & 2){	// Def
			XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  		BlackPixel(dsp, DefaultScreen(dsp)) );

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 8,45,8,49);
			XDrawPoint(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 9,45);
			XDrawPoint(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 9,49);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 10,46,10,48);

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 12,45,12,49);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 13,45,14,45);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 13,47,14,47);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 13,49,14,49);

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 16,45,16,49);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 17,45,18,45);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 17,47,18,47);
		} else {
			XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  		WhitePixel(dsp, DefaultScreen(dsp)) );
			XFillRectangle(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 8,45,11,5);
		}
		
		if(val & 0x10){	// RUN
			XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  		BlackPixel(dsp, DefaultScreen(dsp)) );

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 8,7,8,11);
			XDrawPoint(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 9,7);
			XDrawPoint(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 10,8);
			XDrawPoint(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 9,9);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 10,10,10,11);

			XPoint gU[]={{12,7},{12,11},{14,11},{14,7}};
			XDrawLines(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ),gU,4,CoordModeOrigin);

			XPoint gN[]={{16,11},{16,7},{18,11},{18,7}};
			XDrawLines(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ),gN,4,CoordModeOrigin);
		} else {
			XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  		WhitePixel(dsp, DefaultScreen(dsp)) );
			XFillRectangle(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 8,7,11,5);
		}
		
		if(val & 0x20){	// PRO
			XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  		BlackPixel(dsp, DefaultScreen(dsp)) );

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 8,13,8,17);
			XDrawPoint(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 9,13);
			XDrawPoint(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 10,14);
			XDrawPoint(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 9,15);

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 12,13,12,17);
			XDrawPoint(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 13,13);
			XDrawPoint(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 14,14);
			XDrawPoint(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 13,15);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 14,16,14,17);

			XDrawRectangle(	dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 16, 13, 2, 4);
		} else {
			XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  		WhitePixel(dsp, DefaultScreen(dsp)) );
			XFillRectangle(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 8,12,11,6);
		}

		if(val & 0x40){	// Kana
			XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  		BlackPixel(dsp, DefaultScreen(dsp)) );

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 9,23,12,23);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 12,23,12,26);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 10,22,10,26);

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 14,23,16,23);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 15,22,15,25);
			XDrawPoint(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 14,26);
		} else {
			XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  		WhitePixel(dsp, DefaultScreen(dsp)) );
			XFillRectangle(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 8,22,10,5);
		}

		if(val & 0x80){	// Small
			XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  		BlackPixel(dsp, DefaultScreen(dsp)) );
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 9,28,10,28);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 8,29,11,31);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 8,32,10,32);

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 13,28,13,32);
			XDrawPoint(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 14,29);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 15,28,15,32);

			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 17,28,17,32);
			XDrawLine(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 17,32,19,32);
		} else {
			XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  		WhitePixel(dsp, DefaultScreen(dsp)) );
			XFillRectangle(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), 8,28,12,5);
		}
		return;
	}

	unsigned short x,y=0;

	x=((adr & 0x0f00) >> 9) * 0x1E;	// Compute the column group

	if((val = adr & 0x00ff) < 0x1E)			// 1st segment
		x += val;
	else if(val < 0x3C){		// 3rd segment
		x += val - 0x1E;
		y = 2;
	} else if(val < 0x40)	// hole
		return;
	else if(val < 0x5E){		// 2nd segment
		x += val - 0x40;
		y = 1;
	} else if(val < 0x7C){	// last segment
		x += val - 0x5E;
		y = 3;
	} else
		return;

	x = (x + 20)*2;
	y *= 8;


	val=pread(adr);
	if(!val) {	// empty column
		XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
			WhitePixel(dsp, DefaultScreen(dsp)) );

		XFillRectangle(dsp, XtWindow(draw), DefaultGC( dsp, 
DefaultScreen(dsp)), x, y*2 + 1, 2, 16);
	} else for(int bit=0; bit<8; bit++){
		if(val & 1)
			XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  		BlackPixel(dsp, DefaultScreen(dsp)) );
		else
			XSetForeground( dsp, DefaultGC( dsp, DefaultScreen(dsp) ),
		  		WhitePixel(dsp, DefaultScreen(dsp)) );

		XFillRectangle(dsp, XtWindow(draw), DefaultGC( dsp, DefaultScreen(dsp) ), x, (y+bit)*2 + 1, 2, 2);
		val >>= 1;
	}
	
	if(!norefresh)
		XFlush(dsp);
}

#endif
