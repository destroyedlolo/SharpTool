/*
 *	emul.cxx
 *	(c) L.Faillie	2002
 *
 *	Emulation of CPU instructions
 *
 *	------------- V 2.0 -----------
 *	13/08/2002	Creation
 *	26/09/2017	Add Cycle emulation
 */

#include "emul.hxx"
#include "sharptool.hxx"
#include "address.hxx"
#include "inst.hxx"

#include <unistd.h>

/* CPU frequency in khz = 10^3
 * usleep unit = 10^-6
 * Delay = c * 1000 / frc
 */
inline void cycle( unsigned short int c ){ // Emulates cycles
	if(cpu->frequency){
		useconds_t d = c*1000 / cpu->frequency;
		usleep(d);
	}
}


/*
 * instruction emulation ...
 */

inline void inc(unsigned char adr){
/* Generic function to increment a register
 * update flags : can't be used on P or Q
 */
	cpu->Q=adr;
	if(!++cpu->internal[adr])
		cpu->flags.Z = cpu->flags.C = true;
	else
		cpu->flags.Z = cpu->flags.C = false;
}

inline void dec(unsigned char adr){
/* Generic function to decrement a register
 * update flags : can't be used on P or Q
 */
	cpu->Q=adr;
	cpu->flags.Z = cpu->flags.C = false;

	if(!--cpu->internal[adr])
		cpu->flags.Z = true;
	else if(cpu->internal[adr] == 0xff)
		cpu->flags.C = true;
}

inline void storebin16(unsigned char regnum, unsigned short bin){
/* store a 16 bits values on a pair of registers
 * BUG : Don't run correctly if regnum=$5F. I haven't made this test
 * to speed up this function. It's not a problem as there is no normal
 * circonstances where a 16bits values have to be stored from PORT C.
 */
	cpu->internal[regnum++] = bin & 0xff;
	cpu->internal[regnum] = bin >> 8;
}

inline unsigned short readbin16(unsigned char regnum){
/* read a 16 bits values from a pair of registers
 * BUG : Don't run correctly if regnum=$5F. I haven't made some test
 * to speed up this function. It's not a problem as there is no normal
 * circonstances where a 16bits values have to be read from PORT C.
 */
	return((cpu->internal[regnum+1] << 8) + cpu->internal[regnum]);
}

inline unsigned char updateflg( unsigned short res ){
	if(res & 0xff00)
		cpu->flags.C = true;
	else
		cpu->flags.C = false;
	if(!(res &= 0xff))
		cpu->flags.Z = true;
	else
		cpu->flags.Z = false;

	return(res);
}

inline unsigned char bcd2bin(unsigned char val){
/* Warning : no test made to check if val have a correct BCD value */
	return(((val & 0xf0) >> 4) * 10 + (val & 0x0f));
}

inline unsigned char bin2bcd(unsigned char val){
/* Warning : works only if val <= 99. */
	return (((val / 10) << 4) + (val % 10));
}

bool emul_illegal(){
/* Called on &3F code ...
 * should be use to add some breakpoint or to emulate whole function.
 * as it's machine dependant, it's handled by CPU.
 * if the result is 'false', the emulation stop.
 */
	return(cpu->illegal());
}

bool emul_panic(){
	puts("*E* Unknown CPU instruction");
	return false;
}

bool emul_lii(){
	cycle(4);
	cpu->I(cpu->pread(++(cpu->PC)));
	++(cpu->PC);
	return true;
}

bool emul_lij(){
	cycle(4);
	cpu->J(cpu->pread(++(cpu->PC)));
	++(cpu->PC);
	return true;
}

bool emul_lia(){
	cycle(4);
	cpu->A(cpu->pread(++(cpu->PC)));
	++(cpu->PC);
	return true;
}

bool emul_lib(){
	cycle(4);
	cpu->B(cpu->pread(++(cpu->PC)));
	++(cpu->PC);
	return true;
}

bool emul_ix(){
	cycle(6);
	cpu->DP = cpu->X()+1;
	storebin16(4, cpu->DP);
	cpu->Q=5;
	++(cpu->PC);
	return true;
}

bool emul_dx(){
	cycle(6);
	cpu->DP = cpu->X()-1;
	storebin16(4, cpu->DP);
	cpu->Q=5;
	++(cpu->PC);
	return true;
}

bool emul_iy(){
	cycle(6);
	cpu->DP = cpu->Y()+1;
	storebin16(6, cpu->DP);
	cpu->Q=7;
	++(cpu->PC);
	return true;
}

bool emul_dy(){
	cycle(6);
	cpu->DP = cpu->Y()-1;
	storebin16(6, cpu->DP);
	cpu->Q=7;
	++(cpu->PC);
	return true;
}

bool emul_mvw(){
	cycle(5 + 2*cpu->I());
	for(register char i=cpu->I()+1; i; i--){
		cpu->internal[cpu->P++] = cpu->internal[cpu->Q++];
		cpu->P &= 0x7f;
		cpu->Q &= 0x7f;
	}

	++(cpu->PC);
	return true;
}

bool emul_exw(){
	cycle(6 + 3*cpu->I());
	for(register char i=cpu->I()+1; i; i--){
		unsigned char tmp=cpu->internal[cpu->P];
		cpu->internal[cpu->P++] = cpu->internal[cpu->Q];
		cpu->internal[cpu->Q++] = tmp;
		cpu->P &= 0x7f;
		cpu->Q &= 0x7f;
	}

	++(cpu->PC);
	return true;
}

bool emul_mvb(){
	cycle(5 + 2*cpu->J());
	for(register char i=cpu->J()+1; i; i--){
		cpu->internal[cpu->P++] = cpu->internal[cpu->Q++];
		cpu->P &= 0x7f;
		cpu->Q &= 0x7f;
	}
	
	++(cpu->PC);
	return true;
}

bool emul_exb(){
	cycle(6 + 3*cpu->J());
	for(register char i=cpu->J()+1; i; i--){
		unsigned char tmp=cpu->internal[cpu->P];
		cpu->internal[cpu->P++] = cpu->internal[cpu->Q];
		cpu->internal[cpu->Q++] = tmp;
		cpu->P &= 0x7f;
		cpu->Q &= 0x7f;
	}

	++(cpu->PC);
	return true;
}

bool emul_adn(){
	cycle(7 + 3*cpu->I());
	unsigned short res = bcd2bin(cpu->A());
	cpu->flags.Z = true;

	for(register char i=cpu->I()+1; i; i--){
		res += bcd2bin(cpu->internal[cpu->P]);
		if((cpu->internal[cpu->P--] = bin2bcd( res % 100)))
			cpu->flags.Z = false;
		res /= 100;
		cpu->P &= 0x7f;
	}
	
	if(res)
		cpu->flags.C = true;
	else
		cpu->flags.C = false;
	++(cpu->PC);
	return true;
}

bool emul_sbn(){
	cycle(7 + 3*cpu->I());
	unsigned short res = bcd2bin(cpu->A());
	cpu->flags.Z = true;
	cpu->flags.C = false;
	
	for(register char i=cpu->I()+1; i; i--){
		unsigned char m=bcd2bin(cpu->internal[cpu->P]);

		if(m < res){	// negative number
			cpu->internal[cpu->P] = bin2bcd(100 + m - res);
			cpu->flags.C = true;
			res = 1;
		} else {
			cpu->internal[cpu->P] = bin2bcd(m - res);
			cpu->flags.C = false;
			res = 0;
		}
		if(cpu->internal[cpu->P--])
			cpu->flags.Z = false;
		cpu->P &= 0x7f;
	}


	++(cpu->PC);
	return true;
}

bool emul_adw(){
	cycle(7 + 3*cpu->I());
	unsigned short res = 0;
	cpu->flags.Z = true;

	for(register char i=cpu->I()+1; i; i--){
		res += bcd2bin(cpu->internal[cpu->P]) + bcd2bin(cpu->internal[cpu->Q--]);
		if((cpu->internal[cpu->P--] = bin2bcd( res % 100)))
			cpu->flags.Z = false;
		res /= 100;
		cpu->P &= 0x7f;
		cpu->Q &= 0x7f;
	}
	
	if(res)
		cpu->flags.C = true;
	else
		cpu->flags.C = false;
	++(cpu->PC);
	return true;
}

bool emul_sbw(){
	cycle(7 + 3*cpu->I());
	unsigned short res = 0;
	cpu->flags.Z = true;
	cpu->flags.C = false;
	
	for(register char i = cpu->I()+1; i; i--){
		unsigned char m = bcd2bin(cpu->internal[cpu->P]);
		unsigned char mq = bcd2bin(cpu->internal[cpu->Q--]) + res;

		if(m < mq){	// negative number
			cpu->internal[cpu->P] = bin2bcd(100 + m - mq);
			cpu->flags.C = true;
			res = 1;
		} else {
			cpu->internal[cpu->P] = bin2bcd(m - mq);
			cpu->flags.C = false;
			res = 0;
		}
		if(cpu->internal[cpu->P--])
			cpu->flags.Z = false;
		cpu->P &= 0x7f;
		cpu->Q &= 0x7f;
	}

	++(cpu->PC);
	return true;
}

bool emul_lidp(){
	cycle(8);
	cpu->DP = cpu->pread(++(cpu->PC));
	cpu->DP <<= 8;
	cpu->DP += cpu->pread(++(cpu->PC));
	++(cpu->PC);
	return true;
}

bool emul_libl(){
	cycle(5);
	cpu->DP &= 0xff00;
	cpu->DP |= cpu->pread(++(cpu->PC));
	++(cpu->PC);
	return true;
}

bool emul_lip(){
	cycle(4);
	cpu->P = cpu->pread(++(cpu->PC)) & 0x7f;
	++(cpu->PC);
	return true;
}

bool emul_liq(){
	cycle(4);
	cpu->Q = cpu->pread(++(cpu->PC)) & 0x7f;
	++(cpu->PC);
	return true;
}

bool emul_adb(){
	cycle(5);
	unsigned long m = (unsigned long)readbin16(cpu->P) + (unsigned long)readbin16(2);

	if(!(m & 0xffff))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	if(m & 0xffff0000)
		cpu->flags.C=true;
	else
		cpu->flags.C=false;

	storebin16(cpu->P, m);
	
	cpu->PC++;
	return true;
}

bool emul_sbb(){
	cycle(5);
	unsigned long m = (unsigned long)readbin16(cpu->P) - (unsigned long)readbin16(2);

	if(!(m & 0xffff))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	if(m & 0xffff0000)
		cpu->flags.C=true;
	else
		cpu->flags.C=false;

	storebin16(cpu->P, m);
	
	cpu->PC++;
	return true;
}

bool emul_mvwd(){
	cycle(5 + 4*cpu->I());
	for(register char i=cpu->I()+1; i; i--){
		cpu->internal[cpu->P++] = cpu->read(cpu->DP++);
		cpu->P &= 0x7f;
	}
	
	++(cpu->PC);
	return true;
}

bool emul_exwd(){
	cycle(7 + 6*cpu->I());
	for(register char i=cpu->I()+1; i; i--){
		unsigned char tmp=cpu->internal[cpu->P];
		cpu->internal[cpu->P++] = cpu->read(cpu->DP);
		cpu->write(cpu->DP++,tmp);
		cpu->P &= 0x7f;
	}

	++(cpu->PC);
	return true;
}

bool emul_mvbd(){
	cycle(5 + 4*cpu->J());
	for(register char i=cpu->J()+1; i; i--){
		cpu->internal[cpu->P++] = cpu->read(cpu->DP++);
		cpu->P &= 0x7f;
	}
	
	++(cpu->PC);
	return true;
}

bool emul_exbd(){
	cycle(7 + 6*cpu->J());
	for(register char i=cpu->J()+1; i; i--){
		unsigned char tmp=cpu->internal[cpu->P];
		cpu->internal[cpu->P++] = cpu->read(cpu->DP);
		cpu->write(cpu->DP++,tmp);
		cpu->P &= 0x7f;
	}

	++(cpu->PC);
	return true;
}

bool emul_srw(){
	cycle(5 + cpu->I());
	unsigned char tmp=0;
	for(register char i=cpu->I()+1; i; i--){
		tmp <<= 4;								// shift to carry to the upper cartet
		tmp |= cpu->internal[cpu->P] & 0x0f;	// store the new carry
		cpu->internal[cpu->P] = (cpu->internal[cpu->P] >> 4) | (tmp & 0xf0);
		cpu->P = ++cpu->P & 0x7f;
	}

	++(cpu->PC);
	return true;
}

bool emul_slw(){
	cycle(5 + cpu->I());
	unsigned char tmp=0;
	for(register char i=cpu->I()+1; i; i--){
		tmp >>= 4;
		tmp |= cpu->internal[cpu->P] & 0xf0;
		cpu->internal[cpu->P] = (cpu->internal[cpu->P] << 4) | (tmp & 0x0f);
		cpu->P = --cpu->P & 0x7f;
	}
	
	++(cpu->PC);
	return true;
}

bool emul_film(){
	cycle(5 + cpu->I());
	for(register char i=cpu->I()+1; i; i--){
		cpu->internal[cpu->P++] = cpu->A();
		cpu->P &= 0x7f;
	}

	++(cpu->PC);
	return true;
}

bool emul_fild(){
	cycle(4 + 3*cpu->I());
	for(register char i=cpu->I()+1; i; i--)
		cpu->write(cpu->DP++,cpu->A());

	++(cpu->PC);
	return true;
}

bool emul_ldp(){
	cycle(2);
	cpu->A(cpu->P);

	++(cpu->PC);
	return true;
}

bool emul_ldq(){
	cycle(2);
	cpu->A(cpu->Q);

	++(cpu->PC);
	return true;
}

bool emul_ldr(){
	cycle(2);
	cpu->A(cpu->R);

	++(cpu->PC);
	return true;
}

bool emul_cla(){
	cycle(2);
	cpu->A(0);

	++(cpu->PC);
	return true;
}

bool emul_ixl(){
	cycle(1);
	emul_ix();
	cpu->A(cpu->read(cpu->DP));

	return true;
}

bool emul_dxl(){
	cycle(1);
	emul_dx();
	cpu->A(cpu->read(cpu->DP));

	return true;
}

bool emul_iys(){
	cycle(1);
	emul_iy();
	cpu->write(cpu->DP, cpu->A());

	return true;
}

bool emul_dys(){
	cycle(1);
	emul_dy();
	cpu->write(cpu->DP, cpu->A());

	return true;
}

bool emul_jrnzp(){
	if(!cpu->flags.Z){
		cycle(7);
		cpu->PC += 1 + cpu->pread( cpu->PC+1 );
	} else {
		cycle(4);
		cpu->PC += 2;
	}
	return true;
}

bool emul_jrnzm(){
	if(!cpu->flags.Z){
		cycle(7);
		cpu->PC += 1 - cpu->pread( cpu->PC+1 );
	} else {
		cycle(4);
		cpu->PC += 2;
	}
	return true;
}

bool emul_jrncp(){
	if(!cpu->flags.C){
		cycle(7);
		cpu->PC += 1 + cpu->pread( cpu->PC+1 );
	} else {
		cycle(4);
		cpu->PC += 2;
	}
	return true;
}

bool emul_jrncm(){
	if(!cpu->flags.C){
		cycle(7);
		cpu->PC += 1 - cpu->pread( cpu->PC+1 );
	} else {
		cycle(4);
		cpu->PC += 2;
	}
	return true;
}

bool emul_jrp(){
	cycle(7);
	cpu->PC += 1 + cpu->pread( cpu->PC+1 );
	return true;
}

bool emul_jrm(){
	cycle(7);
	cpu->PC += 1 - cpu->pread( cpu->PC+1 );
	return true;
}

bool emul_loop(){
	cpu->flags.Z = cpu->flags.C = false;

	if(!--cpu->internal[cpu->R])
		cpu->flags.Z = true;
	else if(cpu->internal[cpu->R] == 0xff)
		cpu->flags.C = true;

	if(cpu->flags.C){
		cycle(10);
		cpu->PC +=2;
		cpu->R = (cpu->R + 1) & 0x7F;
	} else {
		cycle(7);
		cpu->PC += 1 - cpu->pread( cpu->PC+1 );
	}

	return true;
}

bool emul_stp(){
	cycle(2);
	cpu->P = cpu->A();

	++(cpu->PC);
	return true;
}

bool emul_stq(){
	cycle(2);
	cpu->Q = cpu->A();

	++(cpu->PC);
	return true;
}

bool emul_str(){
	cycle(2);
	cpu->R = cpu->A();

	++(cpu->PC);	
	return true;
}

bool emul_nop(){
	cycle(2);
	++(cpu->PC);	
	return true;
}

bool emul_push(){
	cycle(3);
	--cpu->R;
	cpu->R &= 0x7f;
	
	cpu->internal[cpu->R] = cpu->A();
	++(cpu->PC);	
	return true;
}

bool emul_data(){
	/* Little strange and unefficient !
	 * it's needed because the target register must be set AFTER
	 * ba is incremented => if P=$02, A must hold the value and not
	 * the new address !
	 */
	unsigned short ba = (cpu->B() << 8) + cpu->A();
	for(register char i=cpu->I()+1; i; i--){
			// 1st : increment the address
		unsigned short ans = ba++;
		cpu->internal[2] = ba & 0xff;
		cpu->internal[3] = ba >> 8;

			// 2nd : set the value
		cpu->internal[cpu->P++] = cpu->pread(ans);
		cpu->P &= 0x7f;
	}

	++(cpu->PC);
	cycle(11 + 4*cpu->I());
	return true;
}

bool emul_rtn(){
	cycle(4);
	cpu->PC = (cpu->internal[cpu->R+1] << 8) + cpu->internal[cpu->R];
	cpu->R = (cpu->R + 2) & 0x7f;
	return true;
}

bool emul_jrzp(){
	if(cpu->flags.Z){
		cycle(7);
		cpu->PC += 1 + cpu->pread( cpu->PC+1 );
	} else {
		cycle(4);
		cpu->PC += 2;
	}
	return true;
}

bool emul_jrzm(){
	if(cpu->flags.Z){
		cycle(7);
		cpu->PC += 1 - cpu->pread( cpu->PC+1 );
	} else {
		cycle(4);
		cpu->PC += 2;
	}
	return true;
}

bool emul_jrcp(){
	if(cpu->flags.C){
		cycle(7);
		cpu->PC += 1 + cpu->pread( cpu->PC+1 );
	} else {
		cycle(4);
		cpu->PC += 2;
	}
	return true;
}

bool emul_jrcm(){
	if(cpu->flags.C){
		cycle(7);
		cpu->PC += 1 - cpu->pread( cpu->PC+1 );
	} else {
		cycle(4);
		cpu->PC += 2;
	}
	return true;
}

bool emul_inci(){
	cycle(4);
	inc(0);

	++(cpu->PC);	
	return true;
}

bool emul_deci(){
	cycle(4);
	dec(0);

	++(cpu->PC);	
	return true;
}

bool emul_inca(){
	cycle(4);
	inc(2);

	++(cpu->PC);	
	return true;
}

bool emul_deca(){
	cycle(4);
	dec(2);

	++(cpu->PC);
	return true;
}

bool emul_adm(){
	cycle(3);
	cpu->internal[cpu->P]=updateflg(cpu->internal[cpu->P] + cpu->A());
	++cpu->PC;
	return true;
}

bool emul_sbm(){
	cycle(3);
	cpu->internal[cpu->P]=updateflg(cpu->internal[cpu->P] - cpu->A());
	++cpu->PC;
	return true;
}

bool emul_anma(){
	cycle(3);
	if(!(cpu->internal[cpu->P] &= cpu->A()))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_orma(){
	cycle(3);
	if(!(cpu->internal[cpu->P] |= cpu->A()))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;	
	return true;
}

bool emul_inck(){
	cycle(4);
	inc(8);

	++(cpu->PC);	
	return true;
}

bool emul_deck(){
	cycle(4);
	dec(8);

	++(cpu->PC);	
	return true;
}

bool emul_incm(){
	cycle(4);
	inc(0x0a);

	++(cpu->PC);	
	return true;
}

bool emul_decm(){
	cycle(4);
	dec(0x0a);

	++(cpu->PC);	
	return true;
}

bool emul_ina(){
	cycle(2);
	if(!(cpu->A(cpu->ina())))
		cpu->flags.Z = 1;
	else
		cpu->flags.Z = 0;

	++(cpu->PC);	
	return true;
}

bool emul_nopw(){
	cycle(2);
	++(cpu->PC);
	return true;
}

bool emul_wait(){
	cycle( 6 + cpu->pread(++(cpu->PC)));
	++(cpu->PC);
	return true;
}

bool emul_cup(){
	++(cpu->PC);
	return true;
}

bool emul_incp(){
	cycle(2);
	++cpu->P;
	cpu->P &= 0x7f;
	
	++(cpu->PC);
	return true;
}

bool emul_decp(){
	cycle(2);
	--cpu->P;
	cpu->P &= 0x7f;
	
	++(cpu->PC);
	return true;
}

bool emul_std(){
	cycle(2);
	cpu->write(cpu->DP, cpu->A());
	++(cpu->PC);
	return true;
}

bool emul_mvmd(){
	cycle(3);
	cpu->internal[cpu->P] = cpu->read(cpu->DP);
	++(cpu->PC);
	return true;
}

bool emul_readm(){
	cycle(3);
	++(cpu->PC);
	cpu->internal[cpu->P] = cpu->read(cpu->PC);
	return true;
}

bool emul_mvdm(){
	cycle(3);
	cpu->write(cpu->DP, cpu->internal[cpu->P]); 
	++(cpu->PC);
	return true;
}

bool emul_read(){
	cycle(3);
	++(cpu->PC);
	cpu->internal[2] = cpu->read(cpu->PC);
	return true;
}

bool emul_ldd(){
	cycle(3);
	cpu->internal[2] = cpu->read(cpu->DP);
	++(cpu->PC);
	return true;
}

bool emul_swp(){
	cycle(2);
	cpu->internal[2] = (cpu->internal[2] << 4) | (cpu->internal[2] >> 4);
	++(cpu->PC);
	return true;
}

bool emul_ldm(){
	cycle(2);
	cpu->internal[2] = cpu->internal[cpu->P];
	++(cpu->PC);
	return true;
}

bool emul_sl(){
	cycle(2);
	bool nvC = cpu->A() & 0x80;
	if(cpu->flags.C)
		cpu->internal[2] = (cpu->A() << 1) | 1;
	else
		cpu->internal[2] <<= 1;
	cpu->flags.C=nvC;
	cpu->flags.Z = !cpu->A();
	++(cpu->PC);
	return true;
}

bool emul_pop(){
	cycle(2);
	cpu->internal[2] = cpu->internal[cpu->R++];
	cpu->R &= 0x7f;

	++(cpu->PC);
	return true;
}

bool emul_outa(){
	cycle(3);
	cpu->outIA();

	++(cpu->PC);
	return true;
}

bool emul_outf(){
	cycle(3);
	cpu->outOutF();
	++(cpu->PC);
	return true;
}

bool emul_anim(){
	cycle(4);
	if(!(cpu->internal[cpu->P] &= cpu->pread(++cpu->PC)))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_orim(){
	cycle(4);
	if(!(cpu->internal[cpu->P] |= cpu->pread(++cpu->PC)))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_tsim(){
	cycle(4);
	if(!(cpu->internal[cpu->P] & cpu->pread(++cpu->PC)))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_cpim(){
	cycle(4);
	unsigned char t=cpu->internal[cpu->P], val=cpu->pread(++cpu->PC);
	if( t < val){
		cpu->flags.Z=false;
		cpu->flags.C=true;
	} else if(t == val){
		cpu->flags.Z=true;
		cpu->flags.C=false;
	} else {
		cpu->flags.Z=false;
		cpu->flags.C=false;
	}
	++cpu->PC;
	return true;
}

bool emul_ania(){
	cycle(4);
	if(!(cpu->internal[2] &= cpu->pread(++cpu->PC)))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_oria(){
	cycle(4);
	if(!(cpu->internal[2] |= cpu->pread(++cpu->PC)))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_tsia(){
	cycle(4);
	if(!(cpu->internal[2] & cpu->pread(++cpu->PC)))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_cpia(){
	cycle(4);
	unsigned char t=cpu->A(), val=cpu->pread(++cpu->PC);
	if(t < val){
		cpu->flags.Z=false;
		cpu->flags.C=true;
	} else if(t == val){
		cpu->flags.Z=true;
		cpu->flags.C=false;
	} else {
		cpu->flags.Z=false;
		cpu->flags.C=false;
	}
	++cpu->PC;
	return true;
}

bool emul_case2(){
	cycle(5 + 7*cpu->nbre_case);
	for(register unsigned char i=cpu->nbre_case; i; i--){
		if(cpu->pread(++cpu->PC) == cpu->A()){
			unsigned short nvPC = cpu->pread(++cpu->PC);
			cpu->PC = (nvPC << 8) + cpu->pread(++cpu->PC);
			cpu->flags.Z = true;
			return(true);
		} else
			cpu->PC += 2;
	}
	
	unsigned short nvPC = cpu->pread(++cpu->PC);
	cpu->PC = (nvPC << 8) + cpu->pread(++cpu->PC);
	cpu->flags.Z = false;
	return(true);
}

bool emul_test(){
	cycle(4);
	unsigned char val = cpu->pread(++cpu->PC);
	cpu->flags.Z=1;

	/* Checks made by this instruction */
	if((val & 8) && cpu->keyboard[KEY_BRK])			// [BRK]
		cpu->flags.Z=0;

	if((val & 0x40) && cpu->flags.reset){	// Reseting
		cpu->flags.Z=0;
		cpu->flags.reset=0;	
	}

	counter CTnow;
	CTnow.set();

	if((val & 0x01) && CTnow - cpu->CT1 >= 512){
		cpu->CT1.set();
		cpu->flags.Z=0;
	}

	if((val & 0x02) && CTnow - cpu->CT1 >= 2){
		cpu->CT2.set();
		cpu->flags.Z=0;
	}

	++cpu->PC;
	return true;
}

bool emul_cdn(){
	++cpu->PC;
	return true;
}

bool emul_adim(){
	cycle(4);
	cpu->internal[cpu->P]=updateflg(cpu->internal[cpu->P] + cpu->pread(++cpu->PC));
	++cpu->PC;

	return true;
}

bool emul_sbim(){
	cycle(4);
	cpu->internal[cpu->P]=updateflg(cpu->internal[cpu->P] - cpu->pread(++cpu->PC));
	++cpu->PC;

	return true;
}

bool emul_adia(){
	cycle(4);
	cpu->A(updateflg(cpu->A() + cpu->pread(++cpu->PC)));
	++cpu->PC;
	return true;
}

bool emul_sbia(){
	cycle(4);
	cpu->A(updateflg(cpu->A() - cpu->pread(++cpu->PC)));
	++cpu->PC;
	return true;
}

bool emul_call(){
	cycle(8);

		// store the target address
	unsigned short nvPC = cpu->pread(++(cpu->PC));
	nvPC = (nvPC << 8) + cpu->pread(++(cpu->PC));

		// push in the stack the return address
	++cpu->PC;
	cpu->internal[cpu->R-1] = cpu->PC >> 8;
	cpu->internal[cpu->R-2] = cpu->PC & 0xff;

	cpu->R = (cpu->R - 2) & 0x7f;

		// jump to the new value
	cpu->PC = nvPC;
	return true;
}

bool emul_jp(){
	cycle(6);
	unsigned short nvPC = cpu->pread(++cpu->PC);
	cpu->PC = (nvPC << 8) + cpu->pread(++cpu->PC);

	return true;
}

bool emul_case1(){
	cycle(9);
	cpu->nbre_case = cpu->pread(++cpu->PC);
	cpu->internal[--cpu->R] = cpu->pread(++cpu->PC);
	cpu->internal[--cpu->R] = cpu->pread(++cpu->PC);
	cpu->R &= 0x7f;

	++cpu->PC;
	return true;
}

bool emul_jpnz(){
	cycle(6);
	if(!cpu->flags.Z){
		unsigned short nvPC = cpu->pread(++(cpu->PC));
		cpu->PC = (nvPC << 8) + cpu->pread(++(cpu->PC));
	} else
		cpu->PC += 3;
	return true;
}

bool emul_jpnc(){
	cycle(6);
	if(!cpu->flags.C){
		unsigned short nvPC = cpu->pread(++(cpu->PC));
		cpu->PC = (nvPC << 8) + cpu->pread(++(cpu->PC));
	} else
		cpu->PC += 3;
	return true;
}

bool emul_jpz(){
	cycle(6);
	if(cpu->flags.Z){
		unsigned short nvPC = cpu->pread(++(cpu->PC));
		cpu->PC = (nvPC << 8) + cpu->pread(++(cpu->PC));
	} else
		cpu->PC += 3;
	return true;
}

bool emul_jpc(){
	cycle(6);
	if(cpu->flags.C){
		unsigned short nvPC = cpu->pread(++(cpu->PC));
		cpu->PC = (nvPC << 8) + cpu->pread(++(cpu->PC));
	} else
		cpu->PC += 3;
	return true;
}

bool emul_lp(){
	cycle(2);
	cpu->P = cpu->pread(cpu->PC++) - 0x80;
	return true;
}

bool emul_incj(){
	cycle(4);
	inc(1);

	++(cpu->PC);
	return true;
}

bool emul_decj(){
	cycle(4);
	dec(1);

	++(cpu->PC);
	return true;
}

bool emul_incb(){
	cycle(4);
	inc(3);

	++(cpu->PC);
	return true;
}

bool emul_decb(){
	cycle(4);
	dec(3);

	++(cpu->PC);
	return true;
}

bool emul_adcm(){
	cycle(3);
	cpu->internal[cpu->P]=updateflg(cpu->internal[cpu->P] + cpu->A() + (cpu->flags.C ? 1:0));
	++cpu->PC;
	return true;
}

bool emul_sbcm(){
	cycle(3);
	cpu->internal[cpu->P]=updateflg(cpu->internal[cpu->P] - cpu->A() - (cpu->flags.C ? 1:0));
	++cpu->PC;
	return true;
}

bool emul_tsma(){
	cycle(3);
	if(!(cpu->internal[cpu->P] & cpu->A()))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_cpma(){
	cycle(3);
	unsigned char m=cpu->internal[cpu->P], a=cpu->A();
	if(m < a){
		cpu->flags.Z=false;
		cpu->flags.C=true;
	} else if(m == a){
		cpu->flags.Z=true;
		cpu->flags.C=false;
	} else {
		cpu->flags.Z=false;
		cpu->flags.C=false;
	}
	++cpu->PC;
	return true;
}

bool emul_incl(){
	cycle(4);
	inc(9);

	++(cpu->PC);
	return true;
}

bool emul_decl(){
	cycle(4);
	dec(9);

	++(cpu->PC);
	return true;
}

bool emul_incn(){
	cycle(4);
	inc(0x0b);

	++(cpu->PC);
	return true;
}

bool emul_decn(){
	cycle(4);
	dec(0x0b);

	++(cpu->PC);
	return true;
}

bool emul_inb(){
	cycle(2);
	if(!(cpu->A(cpu->inb())))
		cpu->flags.Z = 1;
	else
		cpu->flags.Z = 0;

	++(cpu->PC);	
	return true;
}

bool emul_nopt(){
	cycle(3);
	++(cpu->PC);	
	return true;
}

bool emul_sc(){
	cycle(2);
	cpu->flags.C = true;
	cpu->flags.Z = true;
	++(cpu->PC);	
	return true;
}

bool emul_rc(){
	cycle(2);
	cpu->flags.C = false;
	cpu->flags.Z = true;
	++(cpu->PC);	
	return true;
}

bool emul_sr(){
	cycle(2);
	bool nvC = cpu->A() & 0x01;
	if(cpu->flags.C)
		cpu->internal[2] = (cpu->A() >> 1) | 0x80;
	else
		cpu->internal[2] >>= 1;
	cpu->flags.C=nvC;
	cpu->flags.Z = !cpu->A();

	++(cpu->PC);
	return true;
}

bool emul_write(){
	cycle(2);	// Who know ????
	++(cpu->PC);
	return true;
}

bool emul_anid(){
	cycle(6);
	unsigned char res= cpu->read(cpu->DP) & cpu->pread(++cpu->PC);
	if(!res)
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;
	cpu->write(cpu->DP, res);
	cpu->PC++;
	return true;
}

bool emul_orid(){
	cycle(6);
	unsigned char res= cpu->read(cpu->DP) | cpu->pread(++cpu->PC);
	if(!res)
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;
	cpu->write(cpu->DP, res);
	cpu->PC++;
	return true;
}

bool emul_tsid(){
	cycle(6);
	if(!(cpu->read(cpu->DP) & cpu->pread(++cpu->PC)))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_cpid(){
	cycle(6);
	unsigned short t=cpu->read(cpu->DP), val=cpu->pread(++cpu->PC);
	if(t < val){
		cpu->flags.Z=false;
		cpu->flags.C=true;
	} else if(t==val){
		cpu->flags.Z=true;
		cpu->flags.C=false;
	} else {
		cpu->flags.Z=false;
		cpu->flags.C=false;
	}
	++cpu->PC;
	return true;
}

bool emul_leave(){
	cycle(2);
	cpu->internal[cpu->R]=0;

	++(cpu->PC);
	return true;
}

bool emul_exab(){
	cycle(3);
	unsigned char tmp=cpu->A();
	cpu->A(cpu->B());
	cpu->B(tmp);

	++(cpu->PC);
	return true;
}

bool emul_exam(){
	cycle(3);
	unsigned char tmp=cpu->A();
	cpu->A(cpu->internal[cpu->P]);
	cpu->internal[cpu->P] = tmp;

	++(cpu->PC);
	return true;
}

bool emul_outb(){
	cycle(2);
	cpu->outIB();

	++(cpu->PC);
	return true;
}

bool emul_outc(){
	cycle(2);
	cpu->outOutC();

	++(cpu->PC);
	return true;
}

bool emul_cal(){
	cycle(7);
		// store the target address
	unsigned short nvPC = cpu->pread(cpu->PC) - 0xe0;
	nvPC = (nvPC << 8) + cpu->pread(++(cpu->PC));

		// push in the stack the return address
	++cpu->PC;
	cpu->internal[cpu->R-1] = cpu->PC >> 8;
	cpu->internal[cpu->R-2] = cpu->PC & 0xff;

	cpu->R = (cpu->R - 2) & 0x7f;

		// jump to the new value
	cpu->PC = nvPC;
	return true;
}

