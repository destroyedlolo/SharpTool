/*
 *	dump.hxx
 *	(c) L.Faillie	2002
 *
 *	For the moment, it's only a powerfull desassembleur for Sharp Pockets
 *
 *	Desassemble memory
 *
 *	16/04/2002	Creation
 *	------------- V 1.0 -----------
 *	13/08/2002	The instructions table is now in instr.hxx
 */
#ifndef DUMP_HXX
#define DUMP_HXX 1

extern void ex_cmd_disassemble(char *);

// dump arguments functions
extern bool dump_argbyte(unsigned short &, int &);
extern bool dump_argreg(unsigned short &, int &);
extern bool dump_argadr(unsigned short &, int &);
extern bool dump_argpadr(unsigned short &, int &);
extern bool dump_argmadr(unsigned short &, int &);
extern bool dump_case1(unsigned short &, int &);
extern bool dump_case2(unsigned short &, int &nl);
extern bool dump_lp(unsigned short &, int &);
extern bool dump_cal(unsigned short &adr, int &);
#endif
