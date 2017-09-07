/*
 * PC-2500.cxx
 *
 *	cpu redefinition for PC-2500
 *
 *	19/10/2010	Creation
 */

#include "PC-2500.hxx"
#include "sharptool.hxx"

#include <cstdio>
#include <cerrno>

unsigned char pc2500K::ina(){
	unsigned char latch = (pread(0x7A00) & 0x0f) | ((pread(0x7B00) & 0x0f)<<4), res=0;

	if(IA() & 0x01){
		if(keyboard[KEY_MODE])
			res |= 1<<1;
		if(keyboard[KEY_PEN])
			res |= 1<<2;
		if(keyboard['.'])
			res |= 1<<3;
		if(keyboard['L'])
			res |= 1<<4;
		if(keyboard['O'])
			res |= 1<<5;
		if(keyboard['9'])
			res |= 1<<6;
	}
	if(IA() & 0x02){
		if(keyboard[KEY_DOWN])
			res |= 1<<2;
		if(keyboard[';'])
			res |= 1<<4;
		if(keyboard['P'])
			res |= 1<<5;
		if(keyboard['0'])
			res |= 1<<6;
	}
	if(IA() & 0x04){
		if(keyboard[KEY_UP])
			res |= 1<<3;
		if(keyboard['\''])	// => ':'
			res |= 1<<5;
		if(keyboard['-'])
			res |= 1<<6;
	}
	if(IA() & 0x08){
		if(keyboard[KEY_RIGHT])
			res |= 1<<4;
		if(keyboard['['])	// => 'Yen'
			res |= 1<<5;
		if(keyboard['/'])
			res |= 1<<6;
	}
	if(IA() & 0x10){
		if(keyboard[KEY_LEFT])
			res |= 1<<5;
		if(keyboard['\\'])	// => '^'
			res |= 1<<6;
	}
	if(IA() & 0x20){
		if(keyboard[KEY_DEF])
			res |= 1<<6;
	}

	if(latch & 0x80){
		if(keyboard[KEY_CLS])
			res |= 1<<0;
		if(keyboard[KEY_NK9])
			res |= 1<<1;
		if(keyboard[KEY_NKDIV])
			res |= 1<<2;
		if(keyboard[','])
			res |= 1<<3;
		if(keyboard['K'])
			res |= 1<<4;
		if(keyboard['I'])
			res |= 1<<5;
		if(keyboard['8'])
			res |= 1<<6;
		if(keyboard[KEY_OFF])
			res |= 1<<7;
	}
	if(latch & 0x40){
		if(keyboard[KEY_INS])
			res |= 1<<0;
		if(keyboard[KEY_NK8])
			res |= 1<<1;
		if(keyboard[KEY_NK7])
			res |= 1<<2;
		if(keyboard['M'])
			res |= 1<<3;
		if(keyboard['J'])
			res |= 1<<4;
		if(keyboard['U'])
			res |= 1<<5;
		if(keyboard['7'])
			res |= 1<<6;
/*
		res |= 1<<7; // strap export
*/
	}
	if(latch & 0x20){
		if(keyboard[KEY_DEL])
			res |= 1<<0;
		if(keyboard[KEY_NK5])
			res |= 1<<1;
		if(keyboard[KEY_NK4])
			res |= 1<<2;
		if(keyboard['N'])
			res |= 1<<3;
		if(keyboard['H'])
			res |= 1<<4;
		if(keyboard['Y'])
			res |= 1<<5;
		if(keyboard['6'])
			res |= 1<<6;
		if(keyboard[KEY_SHIFT])
			res |= 1<<7;
	}
	if(latch & 0x10){
		if(keyboard[KEY_NK2])
			res |= 1<<1;
		if(keyboard[KEY_NK1])
			res |= 1<<2;
		if(keyboard['B'])
			res |= 1<<3;
		if(keyboard['G'])
			res |= 1<<4;
		if(keyboard['T'])
			res |= 1<<5;
		if(keyboard['5'])
			res |= 1<<6;
/*
		if(...)	// Printer switch
			res |= 1<<7;
*/
	}
	if(latch & 0x08){
/* Illisible !
		if(keyboard[KEY_DEL])
			res |= 1<<1;
*/
		if(keyboard[KEY_NKDOT])
			res |= 1<<1;
		if(keyboard[KEY_NK0])
			res |= 1<<2;
		if(keyboard['V'])
			res |= 1<<3;
		if(keyboard['F'])
			res |= 1<<4;
		if(keyboard['R'])
			res |= 1<<5;
		if(keyboard['4'])
			res |= 1<<6;
	}
	if(latch & 0x04){
		if(keyboard[KEY_ENTER])
			res |= 1<<0;
		if(keyboard[KEY_NKMINUS])
			res |= 1<<1;
		if(keyboard['+'])
			res |= 1<<2;
		if(keyboard['C'])
			res |= 1<<3;
		if(keyboard['D'])
			res |= 1<<4;
		if(keyboard['E'])
			res |= 1<<5;
		if(keyboard['3'])
			res |= 1<<6;
	}
	if(latch & 0x02){
		if(keyboard[KEY_KANA])
			res |= 1<<0;
		if(keyboard[KEY_NK3])
			res |= 1<<1;
		if(keyboard['*'])
			res |= 1<<2;
		if(keyboard['X'])
			res |= 1<<3;
		if(keyboard['S'])
			res |= 1<<4;
		if(keyboard['W'])
			res |= 1<<5;
		if(keyboard['2'])
			res |= 1<<6;
	}
	if(latch & 0x01){
		if(keyboard[KEY_CAPS])
			res |= 1<<0;
		if(keyboard[KEY_NK6])
			res |= 1<<1;
		if(keyboard[' '])
			res |= 1<<2;
		if(keyboard['Z'])
			res |= 1<<3;
		if(keyboard['A'])
			res |= 1<<4;
		if(keyboard['Q'])
			res |= 1<<5;
		if(keyboard['1'])
			res |= 1<<6;
	}

/* if(res) 
printf("*d* [%04lx] l:%02x, a:%02x (%02x, %02x) => %02x\n", time(NULL), latch, IA(), pread(0x7A00), pread(0x7B00), res);
*/
	return res;
}

unsigned char pc2500K::read(unsigned short adr){
/* For the moment, only the lower ROM is hidden.
 * This function should be improved for managing memory hole in &7000->7fff area
 */
	if(adr >= 0x8000){	// Bank 1 switch
		if(this->OutF() & 0x01 || pread(0x7100) & 0x02)
			return(memoryB1[adr]);
	}

		// Main memory : bank 0
	if(adr < 0x2000 || (this->OutF() & 0x01))
		return(adr >> 8);
	return(memory[adr]);
}

unsigned char pc2500K::pread(unsigned short adr){
	if(adr >= 0x8000){	// Bank 1 switch
		if(pread(0x7100) & 0x02)
			return(memoryB1[adr]);
	}

	return(memory[adr]);
}

void pc2500K::patch(){
}


