/*
 *	hexa.cxx
 *	(c) L.Faillie	2002
 *
 *	Hexadecimal manipulation.
 *	No check is done in this conversion function
 *
 *	- V1.0 -
 *	09/04/2002	First version
 */

#include "hexa.hxx"
#include <cctype>

inline unsigned short int digit2bin(char dig) {
	if(isdigit(dig))
		return(dig - '0');
	else
		return(toupper(dig) - 'A' + 10);
}

unsigned short int hexa_2_bin16(const char *ptr) {
	return( (digit2bin(*ptr++) << 12) + (digit2bin(*ptr++) << 8 ) +  (digit2bin(*ptr++) << 4) + digit2bin(*ptr) );
}

unsigned char hexa_2_bin8(const char *ptr) {
	return( (digit2bin(*ptr++) << 4) + digit2bin(*ptr) );
}

inline char bin2digit(char val){
	if(val < 10)
		return(val + '0');
	else
		return(val + 'A' - 10);
}

void bin16_2_hexa(char *ptr, unsigned short int val){
	*ptr++ = bin2digit(val >> 12);
	*ptr++ = bin2digit((val >> 8) & 15);
	*ptr++ = bin2digit((val >> 4) & 15);
	*ptr++ = bin2digit( val & 15);
	*ptr=0;
}

void bin8_2_hexa(char *ptr, unsigned char val){
	*ptr++ = bin2digit((val >> 4) & 15);
	*ptr++ = bin2digit( val & 15);
	*ptr=0;
}
