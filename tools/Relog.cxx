/*
 * Relog.cxx
 *	(c) LFSoft 2003
 *
 *	Change addresses in a binary file.
 *
 *	History
 *	-------
 *	03/11/2003 First version
 *
 *	Syntaxe
 *	-------
 *	Relog AAAA < infile > outfile
 */

#include "sharplib/hexa.hxx"

#include <cstring>
#include <cstdio>
#include <cstdlib>

long int	offset=0;		/* offset b/w the real adresse and the loading one */

int main(int ac, char **av){
	unsigned short int	ldadr, rln=0;
	long int	offset;

	if(ac != 2){
		fprintf(stderr,"*E* Syntaxe : Relog AAAA < infile > outfile\n");
		exit(EXIT_FAILURE);
	}

	ldadr = hexa_2_bin16(av[1]);

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
		if(rln == 1)	// Offset computation
			offset = (long)ldadr - (long)adr;

		char outbuff[8];
		bin16_2_hexa(outbuff, adr + offset);
		printf("%s%s\n", outbuff, buff+4);
	}
	exit(EXIT_SUCCESS);
}
