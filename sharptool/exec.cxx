/*
 *	exec.cxx
 *	(c) L.Faillie	2002
 *
 *	Command dispatcher
 *
 *	10/04/2002	Creation
 *	------------- V 0.2 -----------
 *	19/10/2010	Add loadB1 for 2500
 *	05/12/2010	Add search command, disassemble to show code source 
 *		and dump for memory dump.
 */

#include "sharptool.hxx"
#include "exec.hxx"
#include "load.hxx"
#include "address.hxx"
#include "dump.hxx"
#include "emul.hxx"

#include "sharplib/lectarg.hxx"
#include "sharplib/hexa.hxx"

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <climits>

static void ex_cmd_man( char *);
static void ex_cmd_help( char *);
static void ex_cmd_quest( char *);
static void ex_cmd_exec( char *);

static void ex_cmd_status(char *){
	puts("*I* ----- Current configuration ------");
	printf("*I* verbose is %s\n", verbose ? "activated" : "desactivated");
	printf("*I* the checksum of binary file is %s verified\n", vcs ? "" : "not");
	printf("*I* %d lines are displayed\n", nldisp);
	if(ignLCDOff)
		puts("*I* The LCD is always ON");
	else
		puts("*I* The LCD can be switched OFF");
	if(trace)
		puts("*I* tracing is enabled");
	if(cpu){
		printf("*I* the machine type is %s\n", cpu->type);
		printf("*I* CPU frequency %d khz\n", cpu->frequency);
	} else
		puts("*I* the machine is undefined");
	if(*home)
		printf("*I* Current home directory : '%s'\n", *home);
	else
		puts("*I* The home directory is not set");
}

static void ex_cmd_verbose(char *arg){
	char *opt=(char *)*lect_arg(&arg);
	if(!strcasecmp(opt,"on")){
		verbose=true;
		puts("*I* Verbose turned on");
	} else if(!strcasecmp(opt,"off"))
		verbose=false;
	else
		printf("*E* '%s' is not a valide option (use 'man' for more information) \n", opt);
}

static void ex_cmd_quit(char *){
	puts("");
	exit(EXIT_SUCCESS);
}

static void ex_cmd_echo(char *arg){
	puts(arg);
}

static void ex_cmd_peek(char *arg){
	unsigned short adr=get_addr( arg );
	char buff[5];
	bin16_2_hexa(buff, adr);
	printf("&%s : ", buff);
	bin8_2_hexa(buff, memory[adr]);
	printf("%s\n", buff);
}

static void ex_cmd_poke(char *arg){
	char *val=(char *)*lect_arg(&arg);
	unsigned short adr = get_addr( val );
	
	while(arg){
		val = (char *)*lect_arg(&arg);
		memory[adr++] = get_byte(val);
	}
}

static void ex_cmd_search(char *arg){
	char *val=(char *)*lect_arg(&arg);
	unsigned short adr = get_addr( val );
	char *dt=NULL;			// Data to find
	unsigned short dlen;	// Length of data

	if(!arg){
		puts("*E* data to find missing (use 'man' for more information)");
		return;
	} else if(*arg == '"'){	// string argument
		dt = ++arg;
		dlen = strlen(dt);
		if(dt[dlen-1] == '"')
			--dlen;; 
	} else {	// Reading one or more byte
puts("*F* not implemented");
exit(EXIT_FAILURE);
	}

	if(!dt || !dlen){
		puts("*E* No data to found");
		return;
	}

	do {
		if(memory[adr] == *dt){
			bool found=true;
			for(register unsigned short i=1; i<dlen; i++){
				if(memory[(adr+i) % 0x10000] != dt[i]){
					found=false;
					break;
				}
			}
			if(found){
				printf("Found : &%04X\n", adr);
				return;
			}
		}
	} while(++adr);	// stop when roll back to 0

	puts("Not found");
}

static void ex_cmd_dump(char *arg){
	unsigned short adr = get_addr( arg );
	unsigned char cs=0;

	for(register int l=1; l<nldisp; l++){
		printf("%04X ",adr);
		for(register int i=0; i<8; i++){
			printf("%02X", memory[(adr+i) % 0x10000]);
			cs += memory[(adr+i) % 0x10000];
		}
		printf(":%02X '", cs);
		for(register int i=0; i<8; i++){
			cs = memory[(adr+i) % 0x10000];
			printf("%c", isprint(cs) ? cs : ' ');
		}
		puts("'");

		adr = (adr + 8)% 0x10000;
	}
}
static void ex_cmd_prompt(char *arg){
	prompt = arg;
}

static void ex_cmd_deflab(char *arg){
	char *val=(char *)*lect_arg(&arg);
	short int adr = get_addr( val );

	if(!arg)
		puts("*E* deflab : label name missing !");
	else if(!adr)
		puts("*E* deflab : invalid address !");
	else if(findlabaddr(arg))
		puts("*E* this label already exists !");
	else
		addlab(adr, arg);
}

static void ex_cmd_home(char *arg){
	if(arg)
		home = arg;
	else
		if(*home)
			printf("*I* Current home directory : '%s'\n", *home);
		else
			puts("*I* The home directory is not set");
}

#ifndef NOTRACE
static void ex_cmd_trace(char *arg){
	char *opt=(char *)*lect_arg(&arg);
	if(!strcasecmp(opt,"on")){
		trace=true;
		if(verbose)
			puts("*I* trace turned on");
	} else if(!strcasecmp(opt,"off")){
		trace=false;
		if(trace)
			puts("*I* trace turned off");
	} else
		printf("*E* '%s' is not a valide option (use 'man' for more information) \n", opt);
}
#endif

static void ex_cmd_ignore(char *arg){
	char *opt=(char *)*lect_arg(&arg);
	if(!strcasecmp(opt,"on")){
		ignLCDOff=true;
		if(verbose)
			puts("*I* LCD always ON");
	} else if(!strcasecmp(opt,"off")){
		ignLCDOff=false;
		if(trace)
			puts("*I* LCD can be switch off");
	} else
		printf("*E* '%s' is not a valide option (use 'man' for more information) \n", opt);
}

/*
 * Table of known commands
 */
static struct _defcmd {
	const char *    cmd;        // Command's name
	enum {non=0, oui, peut_etre}    param;  // Parameter ?
	void (*fonc)(char *);// Function to execute
	const char *    aide;       // Some help ?
} tbl_cmd[]= {
	{"\n-- help commands --\n",   _defcmd::peut_etre, NULL, NULL},
	{"?",    _defcmd::peut_etre,   ex_cmd_quest,    "Lists known commands or get information about a command.\n\t? [<cmd>]"},
	{"help",    _defcmd::non,   ex_cmd_help,    "Lists known commands.\n\thelp"},
	{"man", _defcmd::oui,   ex_cmd_man, "Gets info about a command.\n\tman <cmd>"},

	{"\n\n-- memory commands --\n",   _defcmd::peut_etre, NULL, NULL},
	{"load",    _defcmd::oui,   ex_cmd_load,    "Loads a binary file\n\tload <file> [<hexa start_address>]"},
	{"loadB1",    _defcmd::oui,   ex_cmd_loadB1,    "Loads a binary file in bank1\n\tload <file> [<hexa start_address>]"},
	{"peek",    _defcmd::oui,   ex_cmd_peek,    "displays the value of a memory byte\n\tpeek <address>"},
	{"poke",    _defcmd::oui,   ex_cmd_poke,    "modifies memory bytes\n\tpoke <address> <val> [<val> ...]"},
	{"disassemble",	_defcmd::oui,   ex_cmd_disassemble,	"Disassembles memory\n\ndisassemble <address>"},
	{"dump",	_defcmd::oui,   ex_cmd_dump,	"Dumps memory\n\ndump <address>"},
	{"search",	_defcmd::oui,   ex_cmd_search,	"Searchs in memory\n\tsearch <address> \"string\" | <val>" },

	{"\n\n-- label commands --\n",   _defcmd::peut_etre, NULL, NULL},
	{"deflab",	 _defcmd::oui,	ex_cmd_deflab, "defines a new label\n\tdeflab &6C30 FixedVar"},
	{"loadlab",	 _defcmd::oui,	ex_cmd_loadlab, "loads a label definition file\n\tloadlab XY.lab"},

/*
	{"\n\n-- assembler commands --\n",   _defcmd::peut_etre, NULL, NULL},
	{"asm",		_defcmd::oui,	ex_cmd_asm,	"assemble a source code\n\tasm source\ngenerate {source}.bin (binary dump), {source}.lab (global symbole)\nNotez bien : the memory is not modified by this command"},
*/
	{"\n\n-- emulation commands --\n",   _defcmd::peut_etre, NULL, NULL},
	{"call",	_defcmd::oui,	ex_cmd_call,	"runs a program starting at the address given as parameter.\nNotez bien : it will not start the GUI for this"},
	{"boot",	_defcmd::non, ex_cmd_boot,	"Opens the GUI and boot the machine as a full reset"},
	{"powerup",	_defcmd::non, ex_cmd_powerup,	"Opens the GUI and boot the machine as a power up"},
	{"machine",		_defcmd::peut_etre,	ex_cmd_machine,	"sets the machine type\n\tmachine {machine_name}\nWith this command, you can define the type of the PC to emulate.\nUsing 'machine' without argument, you get the list of known machines"},
#ifndef NOTRACE
	{"trace",	_defcmd::oui,	ex_cmd_trace,	"Prints instruction when they are executed (only in GUI mode)\n\ttrace [on|off]"},
#endif
	{"ignore",	_defcmd::oui,	ex_cmd_ignore,	"If set, the LCD is always on (will speed up the emulator on machine where the LCD is always like PC-1350\n\tignore [on|off]"},

	{"\n\n-- misc commands --\n",   _defcmd::peut_etre, NULL, NULL},
	{"@",		_defcmd::oui,	ex_cmd_exec,	"Executes an sharptool's script"},
	{"echo",	_defcmd::oui,	ex_cmd_echo,	"Displays a message"},
	{"status",	_defcmd::non,	ex_cmd_status,	"Displays the current status"},
	{"verbose",    _defcmd::oui,	ex_cmd_verbose,	"verbose mode\n\tverbose [on|off]"},
	{"home",	_defcmd::peut_etre, ex_cmd_home,	"Sets where sharptool can find files\n\twithout argument, it displays the current value"},
	{"prompt",	_defcmd::oui,	ex_cmd_prompt,	"sets the prompt"},
	{"bye",	_defcmd::non,	ex_cmd_quit,	"exits sharptool"},
	{"exit",	_defcmd::non,	ex_cmd_quit,	"exits sharptool"},
	{"quit",	_defcmd::non,	ex_cmd_quit,	"exits sharptool"},

	{NULL,_defcmd::peut_etre,NULL,NULL}
};

static void ex_cmd_help(char *){
	puts(VERSION" commands");

	for(register int i=0; tbl_cmd[i].cmd; i++)
		printf("%s ",tbl_cmd[i].cmd);

	puts("");
}

static void ex_cmd_man(char *arg){
	char *opt=(char *)*lect_arg(&arg);
	for(register int i=0; tbl_cmd[i].cmd; i++)
		if(!strncasecmp(opt,tbl_cmd[i].cmd,strlen(opt))){
			printf("%s : %s\n",opt,tbl_cmd[i].aide ? tbl_cmd[i].aide : "No help");
			return;
		}

	printf("*E* '%s' unknown, see 'help'.\n",opt);
}


static void ex_cmd_quest(char *opt){
	if(opt)
		ex_cmd_man(opt);
	else
		ex_cmd_help(NULL);
}

void exec_command(char *opt){

		// Suppression des espaces devant la command
	while(isspace(*opt)) opt++;

	if(*opt =='#'||!*opt)   // C'est un commentaire
		return;
	
	const char *cmd=*lect_arg(&opt);
	
	for(register int i=0; tbl_cmd[i].cmd; i++)
		if(!strncasecmp(cmd,tbl_cmd[i].cmd,strlen(cmd))){
			if(ftrace) fprintf(ftrace,">%s\n",tbl_cmd[i].cmd);
			if(tbl_cmd[i].fonc){
				if(opt){
					if(tbl_cmd[i].param != _defcmd::non)
						tbl_cmd[i].fonc(opt);
					else
						printf("*E* '%s' needs no argument\n*E* Use 'man' to syntaxe help.\n",tbl_cmd[i].cmd);
			   } else {
					if(tbl_cmd[i].param != _defcmd::oui)
						tbl_cmd[i].fonc(NULL);
					else
						printf("*E* argument needed by '%s'\n*E* Use 'man' to syntaxe help.\n",tbl_cmd[i].cmd);
				}
			} else
				printf("*E* No functions defined for '%s'\n",tbl_cmd[i].cmd);
			return;
		}

	printf("*E* '%s': Unknown command.\n*E* Use '?' or 'help' to see sharptool commands.\n"
		"*E* Use 'man' to syntaxe help.\n",cmd);

	return;
}

void exec_cmd_file(const char *f, bool ignore){
/* Execute the content of a command file
 *	-> f : name of the file
 *	-> ignore : if true, don't print error message if the file can't be open
 */
	FILE *fch=fopen(f,"r");

	if(!fch){
		if(!ignore)
			perror(f);
		return;
	}

	for(;;){
		char l[LINE_MAX];
		fgets(l, LINE_MAX, fch);
		register char *x=strchr(l,'\n');
		if(x) *x=0;
		
		if(feof(fch)) break;
		if(ferror(fch)){
			perror(f);
			exit(EXIT_FAILURE);
		}

		exec_command(l);
	}
}

static void ex_cmd_exec( char *opt){
	lfchaine fname(*lect_arg(&opt));
	filename(fname);	// the file name

	exec_cmd_file(*fname);
}
