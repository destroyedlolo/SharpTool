/*
 *	Bin2Bas.c
 *		(c) LFSoft 1992-2002
 *
 *	Convert binary dump into a Basic program.
 *
 *	History
 *	-------
 *	??/??/19??	- V1.0 First Version
 *	14/03/2002	- Totaly rewriten (more fexible and check the CS)
 *
 *	Syntaxe
 *	-------
 *	Bin2Bas [-n] [-sAAAA] [-lnum] < infile > outfile
 *		-n : doesn't check the CheckSUM
 *		-sAAAA : normaly, data are poked in the adress set in the infile
 *			with this option, the start adresse should be modified
 *			If we have a gaps in the file data are translated according to
 *			the offset vs the start adress in INFILE. i.e :
 *				Bin2Bas -s3000
 *
 *			FILE	->	generated basic
 *			&2000		&3000
 *			&2008		&3008
 *			&2020		&3020
 *
 *			NOTEZ-BIEN : It will trash a machin language program if it's not
 *			PC relative.
 *			AAAA must be different from 0.
 *
 *		-lnum : specify the first line of the basic program (10 default)
 */

#include "sharplib/hexa.hxx"

#include <cstring>
#include <cstdio>
#include <cstdlib>

int vcs=1;				/* Verify the checksum */

unsigned short int	ldadr=0,						/* loading adresse */
					ln=10,							/* line number */
					rln=0;							/* number of the readen line */
long int	offset=0;		/* offset b/w the real adresse and the loading one */

	/** -------------------------------------------------------- **/

int main(int ac, char **av){

		/*
		 *	Arguments reading
		 */
	for(register int i=1;i<ac;i++){
		if(!strcmp(av[i],"-n"))
			vcs=0;
		else if(!strncmp(av[i],"-s",2))
			ldadr = hexa_2_bin16(av[i]+2);
		else if(!strncmp(av[i],"-l",2)){
			if(!(ln = (unsigned short)atoi(av[i]+2))) ln=10;
		} else {
			fprintf(stderr,"*E* Unknown option :'%s'\n", av[i]);
			exit(EXIT_FAILURE);
		}
	}

		/*
		 * Main loop
		 */
	for(;;){
			/* Read the line */
		static char buff[64];
		rln++;
		fgets(buff, 64, stdin);
		if(feof(stdin) || ferror(stdin)){
			if(ferror(stdin)){
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			break;
		}
		if(buff[24] == '\n')
			buff[24]=0;

		if(strlen(buff)!=24){
			fprintf(stderr,"*E* line %d haven't the right size : %d (%s)\n", rln, strlen(buff), buff);
			exit(EXIT_FAILURE);
		}
		if(buff[21] != ':'){
			fprintf(stderr,"*E* line %d haven't the right format (%s)\n", rln, buff);
			exit(EXIT_FAILURE);
		}

			/* the address */
		unsigned short int adr = hexa_2_bin16(buff);
		if(rln == 1 && ldadr)	// Offset computation
			offset = (long)ldadr - (long)adr;

		char outbuff[8];
		bin16_2_hexa(outbuff, adr + offset);
		printf("%d POKE &%s", ln, outbuff);
		ln +=10;

		unsigned char cs=0;		
		for(register int i=0; i<8; i++){
			register unsigned char val=hexa_2_bin8(buff+5+i*2);
			cs += val;
			
			bin8_2_hexa(outbuff, val);
			printf(",&%s", outbuff);
		}
		if(vcs){
			register unsigned char val=hexa_2_bin8(buff+22);
			if(val != cs){
				fprintf(stderr,"*E* line %d haven't the right checksum (file : %02X, calculated %02X)\n", rln, val, cs);
				exit(EXIT_FAILURE);
			}
			
			printf(" : REM %02X", cs);
		}
		puts("");
	}

	exit(EXIT_SUCCESS);
}
