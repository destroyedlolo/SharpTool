/*
 *	sharptool
 *	(c) L.Faillie	2002
 *
 *	For the moment, it's only a powerfull desassembleur for Sharp Pokets
 *
 *	09/04/2002	Creation
 *	------------- V 1.0 -----------
 */
 
#include "sharptool.hxx"
#include "exec.hxx"
#include "GUI_X.hxx"

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <climits>

#include <lflib/lfchaine.hxx>


#ifdef USE_X

#include <cstdlib>

	/* Let a chance to pass X argument to the application */
int argc;
char **argv;

int main(int ac, char **av)
#else
int main(int , char **)
#endif
{
#ifdef USE_X
	argc = ac;
	argv = av;
	
	atexit(clean_X);
#endif

		/* Read the configuration file */
	lfchaine t(getenv("HOME"));
	t += "/.sharptool";
	exec_cmd_file(*t);

	char *x;
	nldisp=0;
	if((x=getenv("LINES")))
		nldisp = atoi(x)-1;
	else
		nldisp = 24;
	if(nldisp < 1)
		nldisp = 1;

		/* Main loop */
	char l[LINE_MAX];
	for(;;){
		printf("%s ", *prompt); fflush(stdout);

		fgets(l,LINE_MAX,stdin);
		if(feof(stdin))
			break;

		x=strchr(l,'\n');
		if(x)	*x=0;

		exec_command(l);
	}

	puts("");
	return(EXIT_SUCCESS);
}
