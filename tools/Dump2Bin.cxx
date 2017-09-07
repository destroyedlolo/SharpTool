/*
 * Dump2Bin.cxx
 *	(c) LFSoft 2010
 *
 *	Convert raw binary file to a binary ...
 *
 *	History
 *	-------
 *	17/10/2010 First version
 *
 *	Syntaxe
 *	-------
 *	Dump2Bin AAAA < infile > outfile
 */

#include "sharplib/hexa.hxx"

#include <cstring>
#include <cstdio>
#include <cstdlib>

int main(int ac, char **av){
	unsigned short int	ldadr;
	unsigned char	checksum;
	long int	offset;
	int cnt;

	if(ac != 2){
		fprintf(stderr,"*E* Syntaxe : Dump2Bin AAAA < infile > outfile\n");
		exit(EXIT_FAILURE);
	}

	ldadr = hexa_2_bin16(av[1]);

		/* Read the file */
	for(cnt=0;;){
		unsigned char c=fgetc(stdin);
		if(feof(stdin))
			break;
		if(!cnt){
			printf("%04X ", ldadr);
			checksum=0;
		}
		printf("%02X", c);
		checksum += c;
		ldadr++;
		if((++cnt) == 8){
			printf(":%02X\n", checksum);
			cnt=0;
		}
	}
}

