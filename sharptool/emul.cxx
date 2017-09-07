/*
 *	emul.cxx
 *	(c) L.Faillie	2002
 *
 *	Emulation of CPU instructions
 *
 *	------------- V 2.0 -----------
 *	13/08/2002	Creation
 */

#include "emul.hxx"
#include "sharptool.hxx"
#include "address.hxx"
#include "inst.hxx"

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
	cpu->I(cpu->pread(++(cpu->PC)));
	++(cpu->PC);
	return true;
}

bool emul_lij(){
	cpu->J(cpu->pread(++(cpu->PC)));
	++(cpu->PC);
	return true;
}

bool emul_lia(){
	cpu->A(cpu->pread(++(cpu->PC)));
	++(cpu->PC);
	return true;
}

bool emul_lib(){
	cpu->B(cpu->pread(++(cpu->PC)));
	++(cpu->PC);
	return true;
}

bool emul_ix(){
	cpu->DP = cpu->X()+1;
	storebin16(4, cpu->DP);
	cpu->Q=5;
	++(cpu->PC);
	return true;
}

bool emul_dx(){
	cpu->DP = cpu->X()-1;
	storebin16(4, cpu->DP);
	cpu->Q=5;
	++(cpu->PC);
	return true;
}

bool emul_iy(){
	cpu->DP = cpu->Y()+1;
	storebin16(6, cpu->DP);
	cpu->Q=7;
	++(cpu->PC);
	return true;
}

bool emul_dy(){
	cpu->DP = cpu->Y()-1;
	storebin16(6, cpu->DP);
	cpu->Q=7;
	++(cpu->PC);
	return true;
}

bool emul_mvw(){
	for(register char i=cpu->I()+1; i; i--){
		cpu->internal[cpu->P++] = cpu->internal[cpu->Q++];
		cpu->P &= 0x7f;
		cpu->Q &= 0x7f;
	}

	++(cpu->PC);
	return true;
}

bool emul_exw(){
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
	for(register char i=cpu->J()+1; i; i--){
		cpu->internal[cpu->P++] = cpu->internal[cpu->Q++];
		cpu->P &= 0x7f;
		cpu->Q &= 0x7f;
	}
	
	++(cpu->PC);
	return true;
}

bool emul_exb(){
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
	cpu->DP = cpu->pread(++(cpu->PC));
	cpu->DP <<= 8;
	cpu->DP += cpu->pread(++(cpu->PC));
	++(cpu->PC);
	return true;
}

bool emul_libl(){
	cpu->DP &= 0xff00;
	cpu->DP |= cpu->pread(++(cpu->PC));
	++(cpu->PC);
	return true;
}

bool emul_lip(){
	cpu->P = cpu->pread(++(cpu->PC)) & 0x7f;
	++(cpu->PC);
	return true;
}

bool emul_liq(){
	cpu->Q = cpu->pread(++(cpu->PC)) & 0x7f;
	++(cpu->PC);
	return true;
}

bool emul_adb(){
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
	for(register char i=cpu->I()+1; i; i--){
		cpu->internal[cpu->P++] = cpu->read(cpu->DP++);
		cpu->P &= 0x7f;
	}
	
	++(cpu->PC);
	return true;
}

bool emul_exwd(){
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
	for(register char i=cpu->J()+1; i; i--){
		cpu->internal[cpu->P++] = cpu->read(cpu->DP++);
		cpu->P &= 0x7f;
	}
	
	++(cpu->PC);
	return true;
}

bool emul_exbd(){
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
	for(register char i=cpu->I()+1; i; i--){
		cpu->internal[cpu->P++] = cpu->A();
		cpu->P &= 0x7f;
	}

	++(cpu->PC);
	return true;
}

bool emul_fild(){
	for(register char i=cpu->I()+1; i; i--)
		cpu->write(cpu->DP++,cpu->A());

	++(cpu->PC);
	return true;
}

bool emul_ldp(){
	cpu->A(cpu->P);

	++(cpu->PC);
	return true;
}

bool emul_ldq(){
	cpu->A(cpu->Q);

	++(cpu->PC);
	return true;
}

bool emul_ldr(){
	cpu->A(cpu->R);

	++(cpu->PC);
	return true;
}

bool emul_cla(){
	cpu->A(0);

	++(cpu->PC);
	return true;
}

bool emul_ixl(){
	emul_ix();
	cpu->A(cpu->read(cpu->DP));

	return true;
}

bool emul_dxl(){
	emul_dx();
	cpu->A(cpu->read(cpu->DP));

	return true;
}

bool emul_iys(){
	emul_iy();
	cpu->write(cpu->DP, cpu->A());

	return true;
}

bool emul_dys(){
	emul_dy();
	cpu->write(cpu->DP, cpu->A());

	return true;
}

bool emul_jrnzp(){
	if(!cpu->flags.Z)
		cpu->PC += 1 + cpu->pread( cpu->PC+1 );
	else
		cpu->PC += 2;
	return true;
}

bool emul_jrnzm(){
	if(!cpu->flags.Z)
		cpu->PC += 1 - cpu->pread( cpu->PC+1 );
	else
		cpu->PC += 2;
	return true;
}

bool emul_jrncp(){
	if(!cpu->flags.C)
		cpu->PC += 1 + cpu->pread( cpu->PC+1 );
	else
		cpu->PC += 2;
	return true;
}

bool emul_jrncm(){
	if(!cpu->flags.C)
		cpu->PC += 1 - cpu->pread( cpu->PC+1 );
	else
		cpu->PC += 2;
	return true;
}

bool emul_jrp(){
	cpu->PC += 1 + cpu->pread( cpu->PC+1 );
	return true;
}

bool emul_jrm(){
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
		cpu->PC +=2;
		cpu->R = (cpu->R + 1) & 0x7F;
	} else
		cpu->PC += 1 - cpu->pread( cpu->PC+1 );

	return true;
}

bool emul_stp(){
	cpu->P = cpu->A();

	++(cpu->PC);
	return true;
}

bool emul_stq(){
	cpu->Q = cpu->A();

	++(cpu->PC);
	return true;
}

bool emul_str(){
	cpu->R = cpu->A();

	++(cpu->PC);	
	return true;
}

bool emul_nop(){
	++(cpu->PC);	
	return true;
}

bool emul_push(){
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
	return true;
}

bool emul_rtn(){
	cpu->PC = (cpu->internal[cpu->R+1] << 8) + cpu->internal[cpu->R];
	cpu->R = (cpu->R + 2) & 0x7f;
	return true;
}

bool emul_jrzp(){
	if(cpu->flags.Z)
		cpu->PC += 1 + cpu->pread( cpu->PC+1 );
	else
		cpu->PC += 2;
	return true;
}

bool emul_jrzm(){
	if(cpu->flags.Z)
		cpu->PC += 1 - cpu->pread( cpu->PC+1 );
	else
		cpu->PC += 2;
	return true;
}

bool emul_jrcp(){
	if(cpu->flags.C)
		cpu->PC += 1 + cpu->pread( cpu->PC+1 );
	else
		cpu->PC += 2;
	return true;
}

bool emul_jrcm(){
	if(cpu->flags.C)
		cpu->PC += 1 - cpu->pread( cpu->PC+1 );
	else
		cpu->PC += 2;
	return true;
}

bool emul_inci(){
	inc(0);

	++(cpu->PC);	
	return true;
}

bool emul_deci(){
	dec(0);

	++(cpu->PC);	
	return true;
}

bool emul_inca(){
	inc(2);

	++(cpu->PC);	
	return true;
}

bool emul_deca(){
	dec(2);

	++(cpu->PC);
	return true;
}

bool emul_adm(){
	cpu->internal[cpu->P]=updateflg(cpu->internal[cpu->P] + cpu->A());
	++cpu->PC;
	return true;
}

bool emul_sbm(){
	cpu->internal[cpu->P]=updateflg(cpu->internal[cpu->P] - cpu->A());
	++cpu->PC;
	return true;
}

bool emul_anma(){
	if(!(cpu->internal[cpu->P] &= cpu->A()))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_orma(){
	if(!(cpu->internal[cpu->P] |= cpu->A()))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;	
	return true;
}

bool emul_inck(){
	inc(8);

	++(cpu->PC);	
	return true;
}

bool emul_deck(){
	dec(8);

	++(cpu->PC);	
	return true;
}

bool emul_incm(){
	inc(0x0a);

	++(cpu->PC);	
	return true;
}

bool emul_decm(){
	dec(0x0a);

	++(cpu->PC);	
	return true;
}

bool emul_ina(){
	if(!(cpu->A(cpu->ina())))
		cpu->flags.Z = 1;
	else
		cpu->flags.Z = 0;

	++(cpu->PC);	
	return true;
}

bool emul_nopw(){
	++(cpu->PC);
	return true;
}

bool emul_wait(){
	cpu->PC += 2;
	return true;
}

bool emul_cup(  ){
return true;}

bool emul_incp(){
	++cpu->P;
	cpu->P &= 0x7f;
	
	++(cpu->PC);
	return true;
}

bool emul_decp(){
	--cpu->P;
	cpu->P &= 0x7f;
	
	++(cpu->PC);
	return true;
}

bool emul_std(){
	cpu->write(cpu->DP, cpu->A());
	++(cpu->PC);
	return true;
}

bool emul_mvmd(){
	cpu->internal[cpu->P] = cpu->read(cpu->DP);
	++(cpu->PC);
	return true;
}

bool emul_readm(){
	++(cpu->PC);
	cpu->internal[cpu->P] = cpu->read(cpu->PC);
	return true;
}

bool emul_mvdm(){
	cpu->write(cpu->DP, cpu->internal[cpu->P]); 
	++(cpu->PC);
	return true;
}

bool emul_read(){
	++(cpu->PC);
	cpu->internal[2] = cpu->read(cpu->PC);
	return true;
}

bool emul_ldd(){
	cpu->internal[2] = cpu->read(cpu->DP);
	++(cpu->PC);
	return true;
}

bool emul_swp(){
	cpu->internal[2] = (cpu->internal[2] << 4) | (cpu->internal[2] >> 4);
	++(cpu->PC);
	return true;
}

bool emul_ldm(){
	cpu->internal[2] = cpu->internal[cpu->P];
	++(cpu->PC);
	return true;
}

bool emul_sl(){
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
	cpu->internal[2] = cpu->internal[cpu->R++];
	cpu->R &= 0x7f;

	++(cpu->PC);
	return true;
}

bool emul_outa(){
	cpu->outIA();

	++(cpu->PC);
	return true;
}

bool emul_outf(){
	cpu->outOutF();
	++(cpu->PC);
	return true;
}

bool emul_anim(){
	if(!(cpu->internal[cpu->P] &= cpu->pread(++cpu->PC)))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_orim(){
	if(!(cpu->internal[cpu->P] |= cpu->pread(++cpu->PC)))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_tsim(){
	if(!(cpu->internal[cpu->P] & cpu->pread(++cpu->PC)))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_cpim(){
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
	if(!(cpu->internal[2] &= cpu->pread(++cpu->PC)))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_oria(){
	if(!(cpu->internal[2] |= cpu->pread(++cpu->PC)))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_tsia(){
	if(!(cpu->internal[2] & cpu->pread(++cpu->PC)))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_cpia(){
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

bool emul_test(  ){
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

bool emul_cdn(  ){
return true;}

bool emul_adim(){
	cpu->internal[cpu->P]=updateflg(cpu->internal[cpu->P] + cpu->pread(++cpu->PC));
	++cpu->PC;

	return true;
}

bool emul_sbim(){
	cpu->internal[cpu->P]=updateflg(cpu->internal[cpu->P] - cpu->pread(++cpu->PC));
	++cpu->PC;

	return true;
}

bool emul_adia(){
	cpu->A(updateflg(cpu->A() + cpu->pread(++cpu->PC)));
	++cpu->PC;
	return true;
}

bool emul_sbia(){
	cpu->A(updateflg(cpu->A() - cpu->pread(++cpu->PC)));
	++cpu->PC;
	return true;
}

bool emul_call(){
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
	unsigned short nvPC = cpu->pread(++cpu->PC);
	cpu->PC = (nvPC << 8) + cpu->pread(++cpu->PC);

	return true;
}

bool emul_case1(){
	cpu->nbre_case = cpu->pread(++cpu->PC);
	cpu->internal[--cpu->R] = cpu->pread(++cpu->PC);
	cpu->internal[--cpu->R] = cpu->pread(++cpu->PC);
	cpu->R &= 0x7f;

	++cpu->PC;
	return true;
}

bool emul_jpnz(){
	if(!cpu->flags.Z){
		unsigned short nvPC = cpu->pread(++(cpu->PC));
		cpu->PC = (nvPC << 8) + cpu->pread(++(cpu->PC));
	} else
		cpu->PC += 3;
	return true;
}

bool emul_jpnc(){
	if(!cpu->flags.C){
		unsigned short nvPC = cpu->pread(++(cpu->PC));
		cpu->PC = (nvPC << 8) + cpu->pread(++(cpu->PC));
	} else
		cpu->PC += 3;
	return true;
}

bool emul_jpz(){
	if(cpu->flags.Z){
		unsigned short nvPC = cpu->pread(++(cpu->PC));
		cpu->PC = (nvPC << 8) + cpu->pread(++(cpu->PC));
	} else
		cpu->PC += 3;
	return true;
}

bool emul_jpc(){
	if(cpu->flags.C){
		unsigned short nvPC = cpu->pread(++(cpu->PC));
		cpu->PC = (nvPC << 8) + cpu->pread(++(cpu->PC));
	} else
		cpu->PC += 3;
	return true;
}

bool emul_lp(){
	cpu->P = cpu->pread(cpu->PC++) - 0x80;
	return true;
}

bool emul_incj(){
	inc(1);

	++(cpu->PC);
	return true;
}

bool emul_decj(){
	dec(1);

	++(cpu->PC);
	return true;
}

bool emul_incb(){
	inc(3);

	++(cpu->PC);
	return true;
}

bool emul_decb(){
	dec(3);

	++(cpu->PC);
	return true;
}

bool emul_adcm(){
	cpu->internal[cpu->P]=updateflg(cpu->internal[cpu->P] + cpu->A() + (cpu->flags.C ? 1:0));
	++cpu->PC;
	return true;
}

bool emul_sbcm(){
	cpu->internal[cpu->P]=updateflg(cpu->internal[cpu->P] - cpu->A() - (cpu->flags.C ? 1:0));
	++cpu->PC;
	return true;
}

bool emul_tsma(){
	if(!(cpu->internal[cpu->P] & cpu->A()))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_cpma(){
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
	inc(9);

	++(cpu->PC);
	return true;
}

bool emul_decl(){
	dec(9);

	++(cpu->PC);
	return true;
}

bool emul_incn(){
	inc(0x0b);

	++(cpu->PC);
	return true;
}

bool emul_decn(){
	dec(0x0b);

	++(cpu->PC);
	return true;
}

bool emul_inb(){
	if(!(cpu->A(cpu->inb())))
		cpu->flags.Z = 1;
	else
		cpu->flags.Z = 0;

	++(cpu->PC);	
	return true;
}

bool emul_nopt(){
	++(cpu->PC);	
	return true;
}

bool emul_sc(){
	cpu->flags.C = true;
	cpu->flags.Z = true;
	++(cpu->PC);	
	return true;
}

bool emul_rc(){
	cpu->flags.C = false;
	cpu->flags.Z = true;
	++(cpu->PC);	
	return true;
}

bool emul_sr(){
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
	++(cpu->PC);
	return true;
}

bool emul_anid(){
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
	if(!(cpu->read(cpu->DP) & cpu->pread(++cpu->PC)))
		cpu->flags.Z=true;
	else
		cpu->flags.Z=false;

	cpu->PC++;
	return true;
}

bool emul_cpid(){
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
	cpu->internal[cpu->R]=0;

	++(cpu->PC);
	return true;
}

bool emul_exab(){
	unsigned char tmp=cpu->A();
	cpu->A(cpu->B());
	cpu->B(tmp);

	++(cpu->PC);
	return true;
}

bool emul_exam(){
	unsigned char tmp=cpu->A();
	cpu->A(cpu->internal[cpu->P]);
	cpu->internal[cpu->P] = tmp;

	++(cpu->PC);
	return true;
}

bool emul_outb(){
	cpu->outIB();

	++(cpu->PC);
	return true;
}

bool emul_outc(){
	cpu->outOutC();

	++(cpu->PC);
	return true;
}

bool emul_cal(){
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

