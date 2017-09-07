/*
 *	lectarg.cxx
 *	(C) L.Faillie	1997
 *
 *	Parse command line
 *
 *	10/04/2002	First version from FGDB's project
 */

#include "lectarg.hxx"

lfchaine lect_arg(char **pos, bool so){
/*  Parse next argument of a command line.
 *  <- res: Argument readen.
 *  -> pos: Start position of argument
 *  -> so: 'true' is argument stops after 2nd '"'
 */
	lfchaine res;

	while(isspace(**pos))    // Skip whitespace
		(*pos)++;

	bool instr=false;    // In string

	for(;;(*pos)++){
		switch(**pos){
		case 0:
			*pos=NULL;
			return res;
		case '"':
			if(instr && so){
				(*pos)++;
				return res;
			} else
				instr = instr ? false:true;
			break;
		case ' ':
		case '\t':
			if(instr) goto ajoute;
			do
				(*pos)++;
			while(isspace(**pos));    // Skip whitespace
			if(!**pos)
				*pos=NULL;
			return res;
		case '\\':
			(*pos)++;
			if(!**pos){  // End of the string
				*pos=NULL;
				return res;
			}
		default:
		ajoute:
			res += **pos;
		}
	}
	
	return res;
}
