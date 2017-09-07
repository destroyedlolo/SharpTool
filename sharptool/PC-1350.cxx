/*
 * PC-1350.cxx
 *
 *	cpu redefinition for PC-1350
 *
 *	14/08/2002	Creation
 *	07/10/2003	Speedup the beast by removing unusefull memory update (update only when
 *		the new value is not the same as the old one).
 */

#include "PC-1350.hxx"
#include "sharptool.hxx"

#include <cstdio>
#include <cerrno>

void pc1350K::refresh_display(){
//	gui_norefresh();
	LCDOff();

	if((OutC() & OUTC_LCD) || ignLCDOff){	// the display is ON

		// Refresh gfx data
		for(unsigned short int seg=0x7000; seg <= 0x7800; seg += 0x0200){
			for(unsigned short off=0; off <= 0x3B; off++)
				updatevideo(seg + off);
			for(unsigned short off=0x40; off <= 0x7B; off++)
				updatevideo(seg + off);
		}
	
		// Refresh indicators
		updatevideo(0x783C);
	}

//	gui_refresh();
}

unsigned char pc1350K::ina(){
/* 
 * Keyboard reading 
 */
	unsigned char res=0, latch;

	if(!(latch = pread(0x7E00))){	// Multiplexing using the port A
		if(IA() & 1){
			if(keyboard['Y'])
				res |= 1<<6;
			if(keyboard['H'])
				res |= 1<<5;
			if(keyboard['N'])
				res |= 1<<4;
			if(keyboard[KEY_DEL])
				res |= 1<<3;
			if(keyboard[KEY_INS])
				res |= 1<<2;
		}
		if(IA() & 2){
			if(keyboard['U'])
				res |= 1<<6;
			if(keyboard['J'])
				res |= 1<<5;
			if(keyboard['M'])
				res |= 1<<4;
			if(keyboard[KEY_MODE])
				res |= 1<<3;
		}
		if(IA() & 4){
			if(keyboard['I'])
				res |= 1<<6;
			if(keyboard['K'])
				res |= 1<<5;
			if(keyboard[' '])
				res |= 1<<4;
			if(keyboard[KEY_CLS])
				res |= 1<<3;
			if(keyboard[KEY_INS])
				res |= 1<<0;
		}
		if(IA() & 8){
			if(keyboard['O'])
				res |= 1<<6;
			if(keyboard['L'])
				res |= 1<<5;
			if(keyboard[KEY_ENTER])
				res |= 1<<4;
			if(keyboard[KEY_CLS])
				res |= 1<<2;
			if(keyboard[KEY_MODE])
				res |= 1<<1;
			if(keyboard[KEY_DEL])
				res |= 1<<0;
		}
		if(IA() & 0x10){
			if(keyboard['P'])
				res |= 1<<6;
			if(keyboard['='])
				res |= 1<<5;
			if(keyboard[KEY_ENTER])
				res |= 1<<3;
			if(keyboard[' '])
				res |= 1<<2;
			if(keyboard['M'])
				res |= 1<<1;
			if(keyboard['N'])
				res |= 1<<0;
		}
		if(IA() & 0x40)	
			if(keyboard[KEY_OFF])
				res |= 0x80;
	} else { 						// Multiplexing using the latch
		if(latch & 1){
			if(keyboard[KEY_SHIFT])
				res |= 1<<6;
			if(keyboard[KEY_DEF])
				res |= 1<<5;
			if(keyboard[KEY_SML])
				res |= 1<<4;
			if(keyboard[','])
				res |= 1<<3;
			if(keyboard[';'])
				res |= 1<<2;
			if(keyboard[':'])
				res |= 1<<1;
			if(keyboard[')'])
				res |= 1<<0;
		}
		if(latch & 2){
			if(keyboard['Q'])
				res |= 1<<6;
			if(keyboard['A'])
				res |= 1<<5;
			if(keyboard['Z'])
				res |= 1<<4;
			if(keyboard['-'])
				res |= 1<<3;
			if(keyboard['*'])
				res |= 1<<2;
			if(keyboard['/'])
				res |= 1<<1;
			if(keyboard['('])
				res |= 1<<0;
		}
		if(latch & 4){
			if(keyboard['W'])
				res |= 1<<6;
			if(keyboard['S'])
				res |= 1<<5;
			if(keyboard['X'])
				res |= 1<<4;
			if(keyboard['+'])
				res |= 1<<3;
			if(keyboard['3'])
				res |= 1<<2;
			if(keyboard['6'])
				res |= 1<<1;
			if(keyboard['9'])
				res |= 1<<0;
		}
		if(latch & 8){
			if(keyboard['E'])
				res |= 1<<6;
			if(keyboard['D'])
				res |= 1<<5;
			if(keyboard['C'])
				res |= 1<<4;
			if(keyboard['.'])
				res |= 1<<3;
			if(keyboard['2'])
				res |= 1<<2;
			if(keyboard['5'])
				res |= 1<<1;
			if(keyboard['8'])
				res |= 1<<0;
		}
		if(latch & 0x10){
			if(keyboard['R'])
				res |= 1<<6;
			if(keyboard['F'])
				res |= 1<<5;
			if(keyboard['V'])
				res |= 1<<4;
			if(keyboard['0'])
				res |= 1<<3;
			if(keyboard['1'])
				res |= 1<<2;
			if(keyboard['4'])
				res |= 1<<1;
			if(keyboard['7'])
				res |= 1<<0;
		}
		if(latch & 0x20){
			if(keyboard['T'])
				res |= 1<<6;
			if(keyboard['G'])
				res |= 1<<5;
			if(keyboard['B'])
				res |= 1<<4;
			if(keyboard[KEY_RIGHT])
				res |= 1<<3;
			if(keyboard[KEY_LEFT])
				res |= 1<<2;
			if(keyboard[KEY_DOWN])
				res |= 1<<1;
			if(keyboard[KEY_UP])
				res |= 1<<0;
		}

/*
		if(latch & 0x40){
			res |= 0x80;	// it's an international machine
		}
*/
	}

	return(res);
}

unsigned char pc1350::ina(){
	if(pread(0x7E00) & 0x40)
		return(0x80 | pc1350K::ina());
	else
		return(pc1350K::ina());
}

unsigned char pc1350K::inb(){
/*
 * CD is always set
 */
	return(0x20);
}

unsigned char pc1350K::read(unsigned short adr){
/* For the moment, only the lower ROM is hidden.
 * This function should be improved for managing memory hole in &7000->7fff area
 */
	if(adr < 0x2000)
		return(adr >> 8);
	return(memory[adr]);
}

unsigned char pc1350K::pread(unsigned short adr){
	return(memory[adr]);
}

void pc1350K::write(unsigned short adr, unsigned char val){
/* For the moment, only ROM are mapped.
 * must be improved to :
 * - map memory holes
 */
	if(adr >= 0x2000 && adr < 0x8000 && memory[adr]!=val ){
		memory[adr]=val;
		if((adr & 0xf000) == 0x7000)
			updatevideo(adr);
	}
}

void pc1350K::patch(){
	/* CLOSE */
	switch(memory[0xFCA5]){
	case 0xD1 :	// Original value - RC -
		memory[0xFCA5] = 0x3F;
		break;
	case 0x3F :	// Already patched
		break;
	default :	// unknown value
		puts("*E* can't patch CLOSE : unexpected value\n*E* is it a PC-1350 ROM 1 ?");
		break;
	}

	/* Read a char from SIO */
	switch(memory[0xF22A]){
	case 0x00 :	// Original value - LII -
		memory[0xF22A] = 0x3F;
		break;
	case 0x3F :	// Already patched
		break;
	default :	// unknown value
		puts("*E* can't patch [ROM 1: receive char SIO] : unexpected value\n*E* is it a PC-1350 ROM 1 ?");
		break;
	}

	/* Check if another char is transmited */
	switch(memory[0xF16F]){
	case 0x03:	// Original value - LIB -
		memory[0xF16F] = 0x3F;
		break;
	case 0x3F :	// Already patched
		break;
	default :	// unknown value
		puts("*E* can't patch [ROM 1: Next char waiting ?] : unexpected value\n*E* is it a PC-1350 ROM 1 ?");
		break;
	}
}

bool pc1350K::illegal(){
	if(PC == 0xFCA5){	// CLOSE : close I/O file if open
		if(fifch){
			fclose(fifch);
			fifch = NULL;
		}
		emul_rc();
		return(emul_rtn());
	} else if(PC == 0xF22A){	// Read char
		if(!fifch){
			if(!*ifch){
				puts("*E* No input file");
				emul_sc();
				return(emul_rtn());
			}
			if(!(fifch = fopen(*ifch,"r"))){
				printf("*E* %s : %s\n", *ifch, strerror(errno));
				emul_sc();
				return(emul_rtn());
			}
		}
		
		P = 0x0E;
		if(feof(fifch))
			internal[0x0e] |= 0x80;
		else {
			A(fgetc(fifch));
			if(A() == '\n')
				internal[0x0e] |= 0x40;
		}
		emul_rc();
		return(emul_rtn());
	} else if(PC == 0xF16F){	// Another char ... NO !
		emul_sc();
		return(emul_rtn());
	} else
		return(CPU::illegal());
}
