/*
 * CheckBin.cxx
 *	(c) LFSoft 2003
 *
 *	Check if a binary file is correct ...
 *	- Addresses must follows each other
 *	- checksum must be ok
 *
 *	History
 *	-------
 *	03/01/2003 First version
 *
 *	Syntaxe
 *	-------
 *	CheckBin [-n] [-a] < infile
 *		-n doesn't check checksums
 *		-a doesn't check addresses
 */

#include "sharplib/hexa.hxx"

#include <cstring>
#include <cstdio>
#include <cstdlib>

bool vcs=true;		/* Verify the checksum */
bool vadr=true;		/* Verify addresses */
short int rln=0;	/* number of the readen line */

int main(int ac, char **av){

		/*
		 *	Arguments reading
		 */
	for(register int i=1;i<ac;i++){
		if(!strcmp(av[i],"-n"))
			vcs=0;
		else if(!strcmp(av[i],"-a"))
			vadr=0;
		else {
			fprintf(stderr,"*E* Unknown option :'%s'\n", av[i]);
			exit(EXIT_FAILURE);
		}
	}

		/*
		 * Main loop
		 */
	unsigned short int ladr = 0;	// last address
	for(;;){
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
		if(buff[21] != ':' || buff[4]!=' '){
			fprintf(stderr,"*E* line %d haven't the right format (%s)\n", rln, buff);
			exit(EXIT_FAILURE);
		}

			/* Address */
		unsigned short int adr = hexa_2_bin16(buff);
		if(ladr && vadr && ladr + 8 != adr){
			fprintf(stderr,"*E* line %d : Non following address (%s)\n", rln, buff);
			exit(EXIT_FAILURE);
		}
		ladr = adr;

		unsigned char cs=0;		
		for(register int i=0; i<8; i++){
			register unsigned char val=hexa_2_bin8(buff+5+i*2);
			cs += val;
		}
		
		if(vcs){
			register unsigned char val=hexa_2_bin8(buff+22);
			if(val != cs){
				fprintf(stderr,"*E* line %d haven't the right checksum (file : %02X, calculated %02X)\n", rln, val, cs);
				exit(EXIT_FAILURE);
			}
		}
	}

	puts("No problem found");
	exit(EXIT_SUCCESS);
}
