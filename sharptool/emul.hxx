/*
 *	emul.hxx
 *	(c) L.Faillie	2002
 *
 *	Assembler
 *
 *	------------- V 2.0 -----------
 *	13/08/2002	Creation
 *	28/11/2010	Change keyboard code to ENUM in order to handle PC-2500
 */
#ifndef EMUL_HXX
#define EMUL_HXX 1

#include "counters.hxx"

#include <lflib/lfchaine.hxx>
#include <cstdlib>
#include <cstdio>

struct CPU {
	CPU() : type(NULL){ init(); };

	virtual ~CPU();

	char keyboard[256];		// Image of the keyboard status

	struct {				// CPU's flags
		unsigned Z:1;
		unsigned C:1;
		unsigned reset: 1;	// we are resetting
	} flags;
	
	unsigned short PC,DP;			// pointers to external memory
	unsigned char internal[128];	// Internal memory
	unsigned char P,Q,R;			// pointers dealing w/ internal memory
	unsigned char nbre_case;		// number of case

	counter CT1;
	counter CT2;

	const char *type;

/* I/O simulation */
	lfchaine	ifch;
	FILE *fifch;

/* Following function will be overwriten by machine's implementation */
	virtual unsigned char ina()=0;
	virtual unsigned char inb()=0;
	virtual unsigned char read(unsigned short)=0;	// normal memory reading (LDD, IXL, ...)
	virtual unsigned char pread(unsigned short)=0;	// privilegied memory reading (DATA, CPU instruction reading, ...)
	virtual void write(unsigned short, unsigned char)=0;	// writing in the memory

	virtual void init();
	virtual void dump();				// dump CPU status

	virtual void gui()=0;				// Called to launch the GUI mode
	void gui_norefresh();				// used on some system to disable screen update when doing strong GFX activities
	void gui_refresh();					// used on some system to reenable screen synchonization.

	virtual void refresh_display()=0;	// Called when the whole display has to be refreshed
	virtual void updatevideo(unsigned short int)=0;	// Update one byte in the video memory
	virtual void LCDOff()=0;	// Switch the screen off (clear every thing but BUSY on some model).

/* This methode is triggered by an 'illegal' instruction (code &3F).
 * It could be use to redirect some routines to a code inside the CPU definition
 * (for example, switch PC's tape reading to emulated file reading).
 */
	virtual bool illegal(){ return false; };
	virtual void patch(){};	// This one patch the ROM to place redirection code ...
	virtual unsigned char customkey(long int);
		// if the PC have special keys, they are translated by this methode
	virtual bool numericKP(){ return false; };
		// True if this PC has separate numeric keypad (i.e. 2500)

	unsigned char I(){ return( internal[0] ); };
	unsigned char I(unsigned char val){ return( internal[0]=val ); };

	unsigned char J(){ return( internal[1] ); };
	unsigned char J(unsigned char val){ return( internal[1]=val ); };

	unsigned char A(){ return( internal[2] ); };
	unsigned char A(unsigned char val){ return( internal[2]=val ); };

	unsigned char B(){ return( internal[3] ); };
	unsigned char B(unsigned char val){ return( internal[3]=val ); };

	unsigned short X(){ return((internal[5] << 8) + internal[4]); };
	unsigned short Y(){ return((internal[7] << 8) + internal[6]); };

	unsigned char K(){ return( internal[8] ); };
	unsigned char K(unsigned char val){ return( internal[8]=val ); };

	unsigned char L(){ return( internal[9] ); };
	unsigned char L(unsigned char val){ return( internal[9]=val ); };

	unsigned char M(){ return( internal[0x0a] ); };
	unsigned char M(unsigned char val){ return( internal[0x0a]=val ); };

	unsigned char N(){ return( internal[0x0b] ); };
	unsigned char N(unsigned char val){ return( internal[0x0b]=val ); };

	unsigned char IA(){ return( cur_a ); };
	unsigned char IA(unsigned char val){ return( internal[0x5c]=val ); };
	unsigned char regIA(){ return(internal[0x5c]); };
	virtual void outIA();

	unsigned char IB(){ return( cur_b ); };
	unsigned char IB(unsigned char val){ return( internal[0x5d]=val ); };
	unsigned char regIB(){ return(internal[0x5d]); };
	virtual void outIB();

	unsigned char OutF(){ return( cur_f ); };
	unsigned char OutF(unsigned char val){ return( internal[0x5e]=val ); };
	unsigned char regOF(){ return(internal[0x5e]); };
	virtual void outOutF();

	unsigned char OutC(){ return( cur_c ); };
	unsigned char OutC(unsigned char val){ return( internal[0x5f]=val ); };
	unsigned char regOC(){ return(internal[0x5f]); };
	virtual void outOutC();
	
private:
	unsigned char cur_a, cur_b, cur_c, cur_f;	// Current out value of port
};

/* Bits for port C */
#define OUTC_LCD	1
#define OUTC_RESET	2
#define	OUTC_HLT	4
#define	OUTC_OFF	8
#define	OUTC_BZ1	(0x10)
#define	OUTC_BZ2	(0x20)
#define	OUTC_BZ3	(0x40)

/* Special keys
 */
enum { 
	KEY_CLS=1, KEY_UP, KEY_DOWN, KEY_BRK, KEY_MODE, KEY_OFF, KEY_INS, KEY_DEL,
	KEY_ENTER, KEY_RIGHT, KEY_LEFT, KEY_SHIFT, KEY_DEF, KEY_SML,

/* 2500 specifics */
	KEY_PEN=128, KEY_KANA, KEY_CAPS, KEY_NKDIV, KEY_NKMINUS, KEY_NKDOT,
	KEY_NK0, KEY_NK1, KEY_NK2, KEY_NK3, KEY_NK4, KEY_NK5, KEY_NK6, KEY_NK7, KEY_NK8, KEY_NK9
};

/*
 * CPU instruction emulation
 */
extern bool emul_illegal();
extern bool emul_panic();
extern bool emul_lii();
extern bool emul_lij();
extern bool emul_lia();
extern bool emul_lib();
extern bool emul_ix();
extern bool emul_dx();
extern bool emul_iy();
extern bool emul_dy();
extern bool emul_mvw();
extern bool emul_exw();
extern bool emul_mvb();
extern bool emul_exb();
extern bool emul_adn();
extern bool emul_sbn();
extern bool emul_adw();
extern bool emul_sbw();
extern bool emul_lidp();
extern bool emul_libl();
extern bool emul_lip();
extern bool emul_liq();
extern bool emul_adb();
extern bool emul_sbb();
extern bool emul_mvwd();
extern bool emul_exwd();
extern bool emul_mvbd();
extern bool emul_exbd();
extern bool emul_srw();
extern bool emul_slw();
extern bool emul_film();
extern bool emul_fild();
extern bool emul_ldp();
extern bool emul_ldq();
extern bool emul_ldr();
extern bool emul_cla();
extern bool emul_ixl();
extern bool emul_dxl();
extern bool emul_iys();
extern bool emul_dys();
extern bool emul_jrnzp();
extern bool emul_jrnzm();
extern bool emul_jrncp();
extern bool emul_jrncm();
extern bool emul_jrp();
extern bool emul_jrm();
extern bool emul_loop();
extern bool emul_stp();
extern bool emul_stq();
extern bool emul_str();
extern bool emul_nop();
extern bool emul_push();
extern bool emul_data();
extern bool emul_rtn();
extern bool emul_jrzp();
extern bool emul_jrzm();
extern bool emul_jrcp();
extern bool emul_jrcm();
extern bool emul_inci();
extern bool emul_deci();
extern bool emul_inca();
extern bool emul_deca();
extern bool emul_adm();
extern bool emul_sbm();
extern bool emul_anma();
extern bool emul_orma();
extern bool emul_inck();
extern bool emul_deck();
extern bool emul_incm();
extern bool emul_decm();
extern bool emul_ina();
extern bool emul_nopw();
extern bool emul_wait();
extern bool emul_cup();
extern bool emul_incp();
extern bool emul_decp();
extern bool emul_std();
extern bool emul_mvmd();
extern bool emul_readm();
extern bool emul_mvdm();
extern bool emul_read();
extern bool emul_ldd();
extern bool emul_swp();
extern bool emul_ldm();
extern bool emul_sl();
extern bool emul_pop();
extern bool emul_outa();
extern bool emul_outf();
extern bool emul_anim();
extern bool emul_orim();
extern bool emul_tsim();
extern bool emul_cpim();
extern bool emul_ania();
extern bool emul_oria();
extern bool emul_tsia();
extern bool emul_cpia();
extern bool emul_case2();
extern bool emul_test();
extern bool emul_cdn();
extern bool emul_adim();
extern bool emul_sbim();
extern bool emul_adia();
extern bool emul_sbia();
extern bool emul_call();
extern bool emul_jp();
extern bool emul_case1();
extern bool emul_jpnz();
extern bool emul_jpnc();
extern bool emul_jpz();
extern bool emul_jpc();
extern bool emul_lp();
extern bool emul_incj();
extern bool emul_decj();
extern bool emul_incb();
extern bool emul_decb();
extern bool emul_adcm();
extern bool emul_sbcm();
extern bool emul_tsma();
extern bool emul_cpma();
extern bool emul_incl();
extern bool emul_decl();
extern bool emul_incn();
extern bool emul_decn();
extern bool emul_inb();
extern bool emul_nopt();
extern bool emul_sc();
extern bool emul_rc();
extern bool emul_sr();
extern bool emul_write();
extern bool emul_anid();
extern bool emul_orid();
extern bool emul_tsid();
extern bool emul_cpid();
extern bool emul_leave();
extern bool emul_exab();
extern bool emul_exam();
extern bool emul_outb();
extern bool emul_outc();
extern bool emul_cal();

/*
 * emul.cxx
 */
extern void ex_cmd_machine(char *);	// set the machine type
extern void ex_cmd_call(char *);	// call a sub routine w/o GUI
extern void ex_cmd_boot(char *);	// boot the machine w/ GUI as a full reset
extern void ex_cmd_powerup(char *);	// boot the machine w/ GUI as a powerup

/*
 * Stuff related to the GUI (i.e. GUI_X.cxx)
 */
extern void gui_powerdown();		// shutdown GUI and return to command line
extern bool have_gui();				// true is the GUI is open
#endif
