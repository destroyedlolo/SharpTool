/*
 *	address.hxx
 *	(c) L.Faillie	2002
 *
 *	For the moment, it's only a powerfull desassembleur for Sharp Pockets
 *
 *	Addresses & labels manipulation
 *
 *	12/04/2002	Creation
 *	------------- V 1.0 -----------
 */

#ifndef ADDR_HXX
#define ADDR_HXX 1

extern unsigned short findlabaddr(const char *);
extern const char *findlabname(unsigned short);
extern void addlab(unsigned short, const char *);

extern unsigned short get_addr(char *);
extern unsigned char get_byte(char *);

#endif
