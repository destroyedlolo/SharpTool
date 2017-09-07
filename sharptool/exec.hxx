/*
 *	exec.hxx
 *	(c) L.Faillie	2002
 *
 *	Command dispatcher
 *
 *	10/04/2002	Creation
 *	------------- V 1.0 -----------
 */
#ifndef EXEC_HXX
#define EXEC_HXX	1

extern void exec_command(char *);
extern void exec_cmd_file(const char *, bool ignore=false);

#endif
