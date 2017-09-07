/*
 *	dump.cxx
 *	(c) L.Faillie	2002
 *
 *	Desassemble memory
 *
 *	16/04/2002	Creation
 *	------------- V 1.0 -----------
 */

#include "sharptool.hxx"
#include "address.hxx"
#include "dump.hxx"
#include "inst.hxx"

#include "sharplib/hexa.hxx"

#include <cctype>
#include <unistd.h>

static char ok_ctn( char &ctn){
	if(!isatty(fileno(stdin)))
		return(ctn='y');

	printf("continue ?(y/n)");
	fflush(stdout);
	do {
		ctn = getchar();
		ctn = tolower(ctn);
		if(ctn == '\n'){
			ctn='y';
			ungetc('\n',stdin);
		}
	} while(ctn !='y' && ctn !='n');

	while(getchar() !='\n');

	return ctn;
}

static void dump_aff_adr( unsigned short adr ){
	const char *lbname;
	
	if(!(lbname=findlabname(adr)))
		printf("&%04X", adr);
	else
		printf("%s", lbname);
}

bool dump_argbyte(unsigned short &adr, int &){
	adr++;
	printf("&%02X", memory[adr]);
	return(true);
}

void dump_aff_reg(unsigned char adr){
	switch(adr){
	case 0:
		printf(" (I)");
		break;
	case 1:
		printf(" (J)");
		break;
	case 2:
		printf(" (A)");
		break;
	case 3:
		printf(" (B)");
		break;
	case 4:
		printf(" (Xl)");
		break;
	case 5:
		printf(" (Xh)");
		break;
	case 6:
		printf(" (Yl)");
		break;
	case 7:
		printf(" (Yh)");
		break;
	case 8:
		printf(" (K)");
		break;
	case 9:
		printf(" (L)");
		break;
	case 0x0A:
		printf(" (M)");
		break;
	case 0x0B:
		printf(" (N)");
		break;
	case 0x10:
		printf(" (Xreg)");
		break;
	case 0x18:
		printf(" (Yreg)");
		break;
	case 0x20:
		printf(" (Zreg)");
		break;
	case 0x28:
		printf(" (Wreg)");
		break;
	case 0x5C:
		printf(" (Port A)");
		break;
	case 0x5D:
		printf(" (Port B)");
		break;
	case 0x5E:
		printf(" (Port F)");
		break;
	case 0x5F:
		printf(" (Port C)");
		break;
	}
}

bool dump_argreg(unsigned short &adr, int &){
	adr++;
	printf("$%02X ", memory[adr]);
	dump_aff_reg(memory[adr]);
	
	return(true);
}

bool dump_argadr(unsigned short &adr, int &){
	adr++;
	dump_aff_adr(memory[adr] * 256 + memory[adr + 1]);
	adr++;
	
	return(true);
}

bool dump_argpadr(unsigned short &adr, int &){
	adr++;
	dump_aff_adr(adr + memory[adr]);
	
	return(true);
}

bool dump_argmadr(unsigned short &adr, int &){
	adr++;
	dump_aff_adr(adr - memory[adr]);
	
	return(true);
}


static unsigned short case_nb=(unsigned short)-1;

bool dump_case1(unsigned short &adr, int &){
	case_nb = memory[++adr];
	printf("%d TEST%s RETURN ", case_nb, case_nb>1 ? "S" : "");
	adr++;
	dump_aff_adr(memory[adr] * 256 + memory[adr + 1]);
	adr++;
	return(true);
}

inline char nextline(int &nl){
	putchar('\n');
	if(++nl >= nldisp){
		char ctn;
		nl=0;
		return(ok_ctn(ctn) == 'y');
	}
	return(true);
}

bool dump_case2(unsigned short &adr, int &nl){
	if(case_nb==(unsigned short)-1){
		puts("\r*E* dump_case2 w/o case1 !\n");
		return(false);
	}
	if(!nextline(nl))
		return(false);
	
	for(register unsigned char i=0; i< (unsigned char)case_nb; i++){
		adr++;
		printf(isprint(memory[adr]) ? "\t  '%c'\t" : "\t  &%02X\t",  memory[adr]);
		adr++;
		dump_aff_adr(memory[adr] * 256 + memory[adr + 1]);
		adr++;
		if(!nextline(nl))
			return(false);
	}
	
	adr++;
	printf("DEFAULT ");
	dump_aff_adr(memory[adr] * 256 + memory[adr + 1]);
	adr++;
	return(true);
}

bool dump_lp(unsigned short &adr, int &){
	printf("$%02X ", memory[adr] - 0x80);
	dump_aff_reg(memory[adr] - 0x80);

	return(true);
}

bool dump_cal(unsigned short &adr, int &){
	unsigned short dest = (((unsigned short)memory[adr] - 0xE0)<<8) + memory[adr+1];
	adr++;
	
	dump_aff_adr(dest);
	return(true);
}

void ex_cmd_disassemble(char *arg){
	unsigned short adr = get_addr( arg );
	char ctn;

	do {
		for(int nl=0; nl < nldisp; nl++){
			const char *lbname;

			if((lbname=findlabname(adr))){
				if(nl >= nldisp-2)
					break;
				printf("%s\n", lbname);
				nl++;
			}

			printf("&%04X\t", adr);

			if(instructions[memory[adr]].name){
				printf("\t%s ", instructions[memory[adr]].name);

				if(instructions[memory[adr]].dmp_fonc)
					if(!instructions[memory[adr]].dmp_fonc(adr, nl))
						return;

				adr++;
				putchar('\n');
			} else {
				printf("\t ERROR : Unknown instruction &%02X\n", memory[adr]);
				return;
			}
		}
		
		ok_ctn(ctn);
	} while( ctn=='y' );

}
