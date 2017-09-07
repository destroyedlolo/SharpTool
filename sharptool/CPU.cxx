/*
 *	CPU.cxx
 *	(c) L.Faillie	2002
 *
 *	Misc CPU stuffs
 *
 *	------------- V 2.0 -----------
 *	13/08/2002	Creation
 *	19/10/2010	Add 2500
 */
#include "emul.hxx"
#include "PC-1350.hxx"
#include "PC-2500.hxx"
#include "sharptool.hxx"
#include "address.hxx"
#include "inst.hxx"

#include <cassert>
#include <cstring>

#define MACHINES_LIST	"1350 1350K 2500 2500K"

/*
 * Sharptool's command
 */

void ex_cmd_machine(char *type){
	if(!type)
		printf("*I* Known machines :%s\n", MACHINES_LIST);
	else {
		if(cpu) delete cpu;
		if(!strcasecmp(type,"1350")){
			cpu=new pc1350;
			assert(cpu);
		} else if(!strcasecmp(type,"1350K")){
			cpu=new pc1350K;
			assert(cpu);
		} else if(!strcasecmp(type,"2500")){
			cpu=new pc2500;
			assert(cpu);
		} else if(!strcasecmp(type,"2500K")){
			cpu=new pc2500K;
			assert(cpu);
		} else
			puts("*E* Unknown machine type");
	}
}

void ex_cmd_call(char *arg){
	if(!cpu){
		puts("*E* No cpu defined : see 'machine' command.");
		return;
	}
	
	cpu->PC=get_addr( arg );
	
	while(instructions[cpu->pread(cpu->PC)].emul_fonc());
	
	cpu->dump();
}

void ex_cmd_boot(char *){
	if(!cpu){
		puts("*E* No cpu defined : see 'machine' command.");
		return;
	}
	cpu->init();
	cpu->PC=0;
	cpu->flags.reset = 1;
	cpu->gui();
}

void ex_cmd_powerup(char *){
	if(!cpu){
		puts("*E* No cpu defined : see 'machine' command.");
		return;
	}

	cpu->init();
	cpu->flags.reset = 0;	
#if 0
	/* Use only for internal testing */
	cpu->PC=0x6c30;
#else
	cpu->PC=0;
#endif
	cpu->gui();
}

/*
 * CPU class function ...
 */

void CPU::init(){
	cur_a=cur_b=cur_c=cur_f=0; 
	R=0x5C;
	J(1);
	memset(keyboard, 0, sizeof(keyboard));
	CT1.set();
	CT2.set();

	fifch=NULL;

	patch();

#ifdef DEBUG
	cur_c=OUTC_LCD;
#endif
}

CPU::~CPU(){
	if(fifch){
		fclose(fifch);
		fifch = NULL;
	}
}

void CPU::dump(){
	puts("*I* ---- CPU Status ----");
	printf("*I* PC : &%04X (PC)= &%02X\n", PC, pread( PC ));
	printf("*I* DP : &%04X", DP);
	if(flags.Z) printf(" Z");
	if(flags.C) printf(" C");
	puts("");
	printf("*I* P: $%02X", P); dump_aff_reg(P);
	printf("  Q: $%02X", Q); dump_aff_reg(Q);
	printf("  R: $%02X\n", R);
	printf("*I* I: &%02X  J: &%02X  A: &%02X  B: &%02X\n", I(), J(), A(), B());
	printf("*I* X: &%04X  Y: &%04X\n", X(), Y());
	printf("*I* K: &%02X  L: &%02X  M: &%02X  N: &%02X\n", K(), L(), M(), N());
	printf("*I* $0c: &%02X  $0d: &%02X  $0e: &%02X  $0f: &%02X\n", internal[0x0c], internal[0x0d], internal[0x0e], internal[0x0f]);
	printf("*I* Xreg : ");
	for(register unsigned char i=0x10;i<=0x17;i++) printf("%02X", internal[i]);
	printf("\n*I* Yreg : ");
	for(register unsigned char i=0x18;i<=0x1F;i++) printf("%02X", internal[i]);
	printf("\n*I* Zreg : ");
	for(register unsigned char i=0x20;i<=0x27;i++) printf("%02X", internal[i]);
	printf("\n*I* Wreg : ");
	for(register unsigned char i=0x28;i<=0x2F;i++) printf("%02X", internal[i]);
	printf("\n*I* $30  : ");
	for(register unsigned char i=0x30;i<=0x37;i++) printf(" %02X", internal[i]);
	printf("\n*I* Stack : ");
	for(register unsigned char i=R;i<=0x5B;i++) printf("%02X", internal[i]);
	puts("");
	printf("*I* IA : &%02X (current output : &%02X), IB : &%02X (current output : &%02X)\n", regIA(), IA(), regIB(), IB());
	printf("*I* OutF : &%02X (current output : &%02X), OutC : &%02X (current output : &%02X)\n", regOF(), OutF(), regOC(), OutC());
}

void CPU::outIA(){
	cur_a = regIA();
}

void CPU::outIB(){
	cur_b = regIB();
}

void CPU::outOutF(){
	cur_f = regOF();
}

void CPU::outOutC(){
	unsigned char ans_c = OutC();
	cur_c = regOC();

	if(OutC() & OUTC_RESET){
		CT1.set();
		CT2.set();
	}
		
	if(OutC() & OUTC_HLT)
		CT1.set();

	if(OutC() & OUTC_OFF)	// Check if the machine is switch off
		gui_powerdown();

	if((OutC() & OUTC_LCD) ^ (ans_c & OUTC_LCD) && !ignLCDOff)	// Check if the LCD status has changed
		refresh_display();
}
