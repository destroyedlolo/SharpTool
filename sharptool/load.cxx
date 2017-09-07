/*
 *	load.cxx
 *	(c) L.Faillie	2002
 *
 *	Configuration
 *
 *	11/04/2002	Creation
 *	06/11/2002	If the 1st character is a '~', it is replaced by the content of home.
 *	------------- V 1.0 -----------
 *	19/10/2010	Add loadB1 for 2500
 */

#include "sharptool.hxx"
#include "load.hxx"
#include "address.hxx"

#include "sharplib/lectarg.hxx"
#include "sharplib/hexa.hxx"

#include <cstdio>
#include <cerrno>
#include <climits>

void internal_load(char *arg,unsigned char *mbank){
	lfchaine fname(*lect_arg(&arg));
	filename(fname);	// the file name

	unsigned short int	ldadr=0,	// loading adress
						rln=0;		// Number of line read
	long int	offset=0;			// offset b/w the real adress and the loading one

	FILE *fch=fopen(*fname,"r");
	if(!fch){
		int oerrno=errno;
		printf("*E* %s : %s\n", *fname, strerror(oerrno));
		if(ftrace)
			fprintf(ftrace,"*E* %s : %s\n", *fname, strerror(oerrno));
		return;
	}

	if(arg)
		ldadr = get_addr(arg);

		/*
		 *	Loading loop
		 */
	for(;;){
		static char buff[64];
		rln++;
		fgets(buff, 64, fch);
		if(feof(fch) || ferror(fch)){
			if(ferror(fch)){
				int oerrno=errno;
				printf("*E* %s : %s\n", *fname, strerror(oerrno));
				if(ftrace)
					fprintf(ftrace,"*E* %s : %s\n", *fname, strerror(oerrno));
				return;
			}
			break;
		}
		if(buff[24] == '\n')
			buff[24]=0;

		if(strlen(buff)!=24){
			printf("*E* line %d haven't the right size : %d (%s)\n", rln, strlen(buff), buff);
			return;
		}
		if(buff[21] != ':'){
			printf("*E* line %d haven't the right format (%s)\n", rln, buff);
			return;
		}

		/* The addresse */
		unsigned short int adr = hexa_2_bin16(buff);
		if(rln == 1 && ldadr)	// Offset computation
			offset = (long)ldadr - (long)adr;
		if(verbose){
			printf("*I* &%04X\r", (unsigned short int)(adr + offset));
		}

		/* Filling the memory */
		unsigned char cs=0;		
		for(register int i=0; i<8; i++){
			register unsigned char val=hexa_2_bin8(buff+5+i*2);
			cs += val;
			mbank[adr + offset + i]=val;
		}
		if(vcs){
			register unsigned char val=hexa_2_bin8(buff+22);
			if(val != cs){
				printf("*E* line %d haven't the right checksum (file : &%02X, calculated &%02X)\n", rln, val, cs);
				return;
			}
		}
	}
	if(verbose)
		puts("");
	fclose(fch);
}

void ex_cmd_load(char *arg){
	internal_load(arg, memory);
}

void ex_cmd_loadB1(char *arg){
	internal_load(arg, memoryB1);
}

void ex_cmd_loadlab(char *arg){
/*
 * This function is quite wired : malformed lines are imply ignored ...
 */
	FILE *fch;

	lfchaine fname(arg);
	filename(fname);	// the file name
	
	if(!(fch=fopen(*fname,"r"))){
		int oerrno=errno;
		printf("*E* %s : %s\n", *fname, strerror(oerrno));
		if(ftrace)
			fprintf(ftrace,"*E* %s : %s\n", *fname, strerror(oerrno));
		return;
	}

	static char buff[LINE_MAX];
	for(;;){
		fgets(buff, LINE_MAX, fch);
		if(feof(fch) || ferror(fch)){
			if(ferror(fch)){
				int oerrno=errno;
				printf("*E* %s : %s\n", *fname, strerror(oerrno));
				if(ftrace)
					fprintf(ftrace,"*E* %s : %s\n", *fname, strerror(oerrno));
				return;
			}
			break;
		}
		char *x=strchr(buff,'\n');
		if(x) *x=0;
		
		if(*buff != '&')
			continue;

		unsigned short adr=hexa_2_bin16(buff+1);
		
		x=strchr(buff,'[');
		if(x){
			char *y=strchr(x,']');
			if(y){
				*(++y)=0;
				addlab(adr, x);
			}
		}
	}
}
