/*
 *	hexa.hxx
 *	(c) L.Faillie	2002
 *
 *	Hexadecimal manipulation.
 *
 */
#ifndef HEXA_HXX
#define HEXA_HXX	1

extern unsigned short int hexa_2_bin16(const char *);
extern unsigned char hexa_2_bin8(const char *);

extern void bin16_2_hexa(char *, unsigned short int);
extern void bin8_2_hexa(char *, unsigned char);

#endif
