/*
 * GUI_X.cxx
 *
 *	Stuff used by a GUI under X windows
 *
 *	06/11/2002	Creation
 *	02/12/2002	Adding some custom checks for HPUX
 *	28/11/2010	Add CPU->NumericKP(), move some keys to 1350's customkey()
 */

#ifdef __unix__

#include "sharptool.hxx"
#include "GUI_X.hxx"
#include "emul.hxx"
#include "inst.hxx"

#include <cstdio>
#include <cerrno>
#include <climits>
#include <cctype>
#include <csetjmp>
#include <X11/keysym.h>
#ifdef __hpux__
#include	<X11/HPkeysym.h>
#endif

#ifdef DEBUG
#include <unistd.h>
#endif

Display *dsp=NULL;

bool norefresh;

static XtAppContext app;
Widget top_wnd, draw;

jmp_buf jmp_ctx;

/* 
 * CPU:: methode linked to the GUI
 */
void CPU::gui_norefresh(){
	norefresh = true;
}

void CPU::gui_refresh(){
	norefresh=false;
	XFlush(dsp);
}

unsigned char CPU::customkey(long int code){
	if(code == XK_Shift_L)
		return(KEY_SHIFT);
	return(0);
}

/*
 * X Stuffs
 */

void clean_X(){
		// Restore auto_repeat
	if(dsp){
		XAutoRepeatOn(dsp);
		XSync(dsp,TRUE);
	}
}

void refreshwnd( Widget, XtPointer,	XtPointer){
	cpu->refresh_display();
}

void enterwnd( Widget, XtPointer,	XEvent*, Boolean *){
	XAutoRepeatOff(dsp);
}

void leavewnd( Widget, XtPointer,	XEvent*, Boolean *){
	XAutoRepeatOn(dsp);
}

#if 0
void closewnd( Widget, XtPointer,	XEvent *aev, Boolean *){
	if(aev->type == DestroyNotify)
		puts("bye");
}
#endif

void keyhandler ( Widget, XtPointer, XEvent *aEv, Boolean * ){
	/* in order to make the machine reactive, the CPU is wakeup if a key 
	 * is pressed.
	 * NOTEZ-BIEN : to speed up things, I don't check if it's really an used key.
	 */
	if(cpu->OutC() & OUTC_HLT){
		cpu->OutC( cpu->OutC() & ~OUTC_HLT );
		cpu->outOutC();
	}

	/* Key reading */
	KeySym ks=XKeycodeToKeysym(dsp, ((XKeyEvent*)aEv)->keycode, 0);
#ifdef __hpux__
	/* Because an HP-UX workstation (i.e. my HP-715) doesn't sent correct code
	 * but only custom one
	 */
	switch(ks){
	case hpXK_InsertChar	: ks = XK_Insert; break;
	case hpXK_DeleteChar	: ks = XK_Delete; break;
	case hpXK_BackTab	: ks = XK_Tab; break;
	}
#endif
	if(ks<256){
		unsigned char res = cpu->customkey((long int)ks);
		if( res )
			ks = (KeySym)res;
		else
		ks = toupper(ks);
	} else {
		KeySym ansks=ks;
		if(!(ks=XKeycodeToKeysym(dsp, ((XKeyEvent*)aEv)->keycode, 2)))
			ks = ansks;
		else if(ks > 0x10000)
			ks = ansks;
		switch(ks){	// Special mapping
		case	XK_F1		: ks = KEY_CLS; break;
		case	XK_Up		: ks = KEY_UP; break;
		case	XK_Down		: ks = KEY_DOWN; break;
		case	XK_Escape	: ks = KEY_BRK; break;
		case	XK_Menu		:	// Because my HP715 doesn't have an HELP key
		case	XK_Home		:	// More obvious on PC keyboard
		case	XK_Help		: ks = KEY_MODE; break;
		case	XK_F8		: ks = KEY_OFF; break;
		case	XK_Return	: ks = KEY_ENTER; break;
		case	XK_Right	: ks = KEY_RIGHT; break;
		case	XK_Left		: ks = KEY_LEFT; break;
		case	XK_Shift_L	: ks = KEY_SHIFT; break;
		case	XK_Tab		: ks = KEY_DEF; break;
		case	XK_Meta_L	:
			ks = KEY_SML; break;
			// No mapped to any key, but used for the emulator itself
		case	XK_F5	: if(((XKeyEvent*)aEv)->type == KeyPress){
				if(cpu->fifch){
					fclose(cpu->fifch);
					cpu->fifch = NULL;
				}
				
				printf("*I* Current input file :'%s'\nNew value ('!' to keep the old value):", *cpu->ifch ? *cpu->ifch : "");
				fflush(stdout);
				char tmp[LINE_MAX];
				scanf("%s", tmp);
				if(*tmp != '!'){
					cpu->ifch = tmp;
					filename(cpu->ifch);
				}
				printf("*I* The input file name is '%s'\n", *cpu->ifch ? *cpu->ifch : "");
			}
			break;
		#ifndef NOTRACE
		case	XK_Execute	:
		case	XK_Print	:
			if(((XKeyEvent*)aEv)->type == KeyPress)
				trace = trace ? false : true;
			return;
		#endif
		default :
 printf("*D* %04x\n", ks);
		if(!cpu->numericKP()){
				switch(ks){
			// numeric key
				case	XK_KP_Insert :
				case	XK_KP_0	: ks = '0'; break;
				case	XK_KP_End :
				case	XK_KP_1	: ks = '1'; break;	
				case	XK_KP_Down :
				case	XK_KP_2	: ks = '2'; break;
				case	XK_KP_Next :
				case	XK_KP_3	: ks = '3'; break;
				case	XK_KP_Left :
				case	XK_KP_4	: ks = '4'; break;
				case	XK_KP_Begin :
				case	XK_KP_5	: ks = '5'; break;
				case	XK_KP_Right :
				case	XK_KP_6	: ks = '6'; break;
				case	XK_KP_Home :
				case	XK_KP_7	: ks = '7'; break;
				case	XK_KP_Up :
				case	XK_KP_8	: ks = '8'; break;
				case	XK_KP_Prior :
				case	XK_KP_9	: ks = '9'; break;
				case	XK_KP_Delete :
				case	XK_KP_Decimal : ks = '.'; break;
				case	XK_KP_Equal	: ks = '='; break;
				case	XK_KP_Multiply	: ks = '*'; break;
				case	XK_KP_Add		: ks = '+'; break;
				case	XK_KP_Subtract	: ks = '-'; break;
				case	XK_KP_Divide	: ks = '/'; break;
				case	XK_KP_Enter	: ks = KEY_ENTER; break;
				default:
					ks = (KeySym)cpu->customkey((long int)ks);
				}
			} else
				ks = (KeySym)cpu->customkey((long int)ks);
		}
	}

	cpu->keyboard[ks] = (((XKeyEvent*)aEv)->type == KeyPress);
}

Boolean cb_run( XtPointer ){
/* Callback called when the program is idle waiting for events.
 * run 1 CPU instruction at once.
 */

	if(cpu->OutC() & OUTC_HLT){
		counter CTnow;
		CTnow.set();
		
		if(CTnow - cpu->CT1 >= 512){
			cpu->OutC( cpu->OutC() & ~OUTC_HLT );
			cpu->outOutC();
		} else {
			timespec ts;
  			ts.tv_sec  = 0;
			ts.tv_nsec = 1000;	// 1 ms
			nanosleep( &ts, NULL );
			return False;
		}
	}

#ifndef NOTRACE
	/* Using NOTRACE, it's possible to speed up a little the beast by removing
 	 * this test.
 	 */
	if(trace){
		printf("&%04x : %s\n", cpu->PC, instructions[cpu->pread(cpu->PC)].name);
	}
#endif

	if(!(instructions[cpu->pread(cpu->PC)].emul_fonc())){
		/* Stop execution any other instruction
		 * Turn OFF the machine or execute an untraped illegal instruction.
		 */
		cpu->dump();
#ifdef DEBUG
		sleep(1);
#endif
		gui_powerdown();
	}

#if 0	// Only used to debug the CPU core
#ifndef NOTRACE
	if(trace)
		cpu->dump();
#endif
#endif

	return False;
}

void gui_powerdown(){
/* Calls at power off.
 */
	
	XDestroyWindow(dsp, XtWindow(top_wnd));	// Close the GUI
	XAutoRepeatOn(dsp);						// Keep thing as we found them
	XSync(dsp,TRUE);
	dsp=NULL;
	longjmp(jmp_ctx,1);						// exit event loop
}

bool have_gui(){
	return((bool)dsp);
}

void gui_init(short int asx, short int asy, const char *atitle="Sharptool"){
	norefresh=false;

	top_wnd = XtVaAppInitialize(&app, atitle, NULL, 0,
		&argc, argv, NULL,
		XmNwidth, asx,
		XmNheight, asy,
	NULL);
	
	dsp = XtDisplay(top_wnd);
	
	Widget main_wnd = XtVaCreateManagedWidget("main_window", xmMainWindowWidgetClass, top_wnd, NULL);

	draw = XtVaCreateWidget("draw", xmDrawingAreaWidgetClass, main_wnd, NULL);

	XtAddCallback(draw, XmNexposeCallback, refreshwnd, NULL);

	XtAddEventHandler( draw, KeyPressMask|KeyReleaseMask,False,
		keyhandler, NULL );	// Why it doesn't work on top_wnd ???
	XtAddEventHandler( top_wnd, EnterWindowMask, False, enterwnd, NULL);
	XtAddEventHandler( top_wnd, LeaveWindowMask, False, leavewnd, NULL);
#if 0
	XtAddEventHandler( top_wnd, StructureNotifyMask, False, closewnd, NULL);
#endif
	XtManageChild(draw);

	XtRealizeWidget(top_wnd);
	XtAppAddWorkProc( app, cb_run, NULL );
	
	if(!setjmp(jmp_ctx))
		XtAppMainLoop(app);
}

#if 0
bool loadskin(const char *afch){	// Set a new skin
	lfchaine fname(home);
	if(*fname)
		fname += '/';
	fname += "skin/";
	fname += afch;

	FILE *fd;
	if(!(fd = fopen(*fname, "rb"))){
		printf("*E* %s : %s\n", *fname, strerror(errno));
		return false;
	}

		/* 
		 * Read data inside the skin file
		 */
	char buf[LINE_MAX];
	
	fgets( buf, 4, fd );	// Read file header
	if(buf[0] != 'P' || buf[1] !='6' ){
		printf("*E* %s : not a PPM file\n", *fname);
		fclose(fd);
		return false;
	}

	fgets( buf, LINE_MAX - 1, fd );
  	if( buf[0]=='#' )
	    fgets( buf, LINE_MAX - 1, fd );

	if(feof(fd)){
		fclose(fd);
		return false;
	}

	sscanf(buf,"%u %u\n", &sx, &sy);
	if(!sx || !sy){
		printf("*E* %s : skin have a bad size (%d, %d)\n", *fname, sx, sy);
		fclose(fd);
		return(false);
	}

	fgets( buf, 5, fd );

	if(skin)
		XFreePixmap(dsp, skin);

	skin = XCreatePixmap( dsp, DefaultRootWindow( dsp ), sx, sy, DefaultDepth(dsp, DefaultScreen(dsp)));


	return true;
}
#endif

#endif
