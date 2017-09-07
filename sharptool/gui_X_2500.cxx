/*
 * gui_X_2500.cxx
 *
 *	GUI for PC-2500 under X windows
 *	Almost everything is done in PC-1350's one. 
 *
 *	28/11/2010	Creation
 */
#ifdef __unix__

#include "PC-2500.hxx"
#include "GUI_X.hxx"
#include "sharptool.hxx"

#include <cstdio>
#include <X11/keysym.h>

unsigned char pc2500K::customkey(long int code){
	switch(code){
	case	XK_Insert	: return(KEY_INS);
	case	XK_BackSpace :
	case	XK_Delete	: return(KEY_DEL);
	case	XK_Shift_L	: return(KEY_SHIFT);
	case	XK_Super_L	: return(KEY_KANA);
	case	XK_Caps_Lock	: return(KEY_CAPS);
	case	XK_KP_Insert :
	case	XK_KP_0	: return(KEY_NK0);
	case	XK_KP_End :
	case	XK_KP_1	: return(KEY_NK1);
	case	XK_KP_Down :
	case	XK_KP_2	: return(KEY_NK2);
	case	XK_KP_Next :
	case	XK_KP_3	: return(KEY_NK3);
	case	XK_KP_Left :
	case	XK_KP_4	: return(KEY_NK4);
	case	XK_KP_Begin :
	case	XK_KP_5	: return(KEY_NK5);
	case	XK_KP_Right :
	case	XK_KP_6	: return(KEY_NK6);
	case	XK_KP_Home :
	case	XK_KP_7	: return(KEY_NK7);
	case	XK_KP_Up :
	case	XK_KP_8	: return(KEY_NK8);
	case	XK_KP_Prior :
	case	XK_KP_9	: return(KEY_NK9);
	case	XK_KP_Delete :
	case	XK_KP_Decimal	: return(KEY_NKDOT);
	case	XK_KP_Subtract	: return(KEY_NKMINUS);
	case	XK_KP_Divide	: return(KEY_NKDIV);
	case	XK_KP_Equal		: return('=');
	case	XK_KP_Multiply	: return('*');
	case	XK_KP_Add		: return('+');
	case	XK_KP_Enter		: return(KEY_ENTER);
	}
	return(0);
}

#endif

