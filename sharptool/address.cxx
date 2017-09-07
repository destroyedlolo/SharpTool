/*
 *	address.cxx
 *	(c) L.Faillie	2002
 *
 *	For the moment, it's only a powerfull desassembleur for Sharp Pockets
 *
 *	Addresses & label manipulation
 *
 *	12/04/2002	Creation
 *	------------- V 1.0 -----------
 */

#include "address.hxx"
#include "lflib/lfchaine.hxx"

#include "sharplib/hexa.hxx"

#include <cctype>
#include <cstdio>
#include <cstdlib>

struct _deflab {
	_deflab *next;
	unsigned short adr;
	lfchaine name;
	
	_deflab(unsigned short , const char *);
} *lablist=NULL;

_deflab::_deflab(unsigned short aadr , const char *aname)
: adr(aadr), name(aname) {
	next = lablist;
	lablist = this;
}

unsigned short findlabaddr(const char *aname){
	for(_deflab *lab=lablist; lab; lab=lab->next){
		if(lab->name == aname)
			return(lab->adr);
	}
	
	return(0);	// Not found
}

const char *findlabname(unsigned short aadr){
	for(_deflab *lab=lablist; lab; lab=lab->next){
		if(lab->adr == aadr)
			return(*lab->name);
	}
	
	return(0);	// Not found
}

void addlab(unsigned short aadr, const char *aname){
	new _deflab(aadr, aname);
}

unsigned short get_addr(char *adr){
/* Read an adress.
 * If the string start w/
 *	& : read an hexadecimal address (&6C30)
 * 0-9 : read a decimal address (27696)
 * [ or { : read a label [Start Var]
 */
	if(*adr == '&')
		return(hexa_2_bin16(++adr));
	else if(isdigit(*adr))
		return((unsigned short)atoi(adr));
	else if(*adr == '[')
		return(findlabaddr(adr));
	else {
		printf("*E* I duno this address format ('%s'): returning NULL\n",adr);				return(0);
	}
}

unsigned char get_byte(char *arg){
/* Read a byte.
 * If the string start w/
 *	& : read an hexadecimal address (&1a)
 * 0-9 : read a decimal address (36)
 */
	if(*arg == '&')
		return(hexa_2_bin8(++arg));
	else if(isdigit(*arg))
		return((unsigned char)atoi(arg));
	else {
		printf("*E* I duno this address format ('%s'): returning NULL\n",arg);				return(0);
	}
}
