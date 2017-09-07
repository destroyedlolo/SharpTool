/*
 *	inst.hxx
 *	(c) L.Faillie	2002
 *
 *	CPU instructions' definition
 *
 *	------------- V 2.0 -----------
 *	13/08/2002	Creation
 */
#ifndef INST_HXX
#define INST_HXX 1

#include "emul.hxx"

extern const struct instruction {
	const char *name;
	bool (* dmp_fonc)(unsigned short &, int &);
	bool (* emul_fonc)();
} instructions[];

#endif
