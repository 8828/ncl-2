#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <ncarg/hlu/hlu.h>
#include <ncarg/hlu/NresDB.h>
#include "defs.h"
#include "Symbol.h"
#include "NclData.h"
#include "Machine.h"
#include <unistd.h>

FILE *thefptr;
FILE *theoptr;
int cmd_line;
extern int cur_line_number;

#ifdef SunOs
extern FILE *nclin;
extern int nclparse();
#else
extern FILE *yyin;
extern int yyparse();
#endif /*SunOs*/

#define BUFF_SIZE 512


main() {

#ifdef YYDEBUG
	extern int _yydebug;
/*		
	extern FILE * _yyerfp;
*/
	_yydebug = 1;
/*
	_yyerfp = fopen("ncl.trace","w");
*/

#endif

	cmd_line =isatty(fileno(stdin));

	thefptr = fopen("ncl.tree","w");
	theoptr = fopen("ncl.seq","w");
	NhlOpen();
	_NclInitMachine();
	_NclInitSymbol();	

	if(cmd_line)	
		fprintf(stdout,"ncl %d> ",0);
#ifdef SunOs
	nclparse();
#else
	yyparse();
#endif
	fclose(thefptr);
	fprintf(stdout,"Number of unfreed objects %d\n",_NclNumObjs());
/*
	_NclPrintUnfreedObjs(stdout);
*/
	NhlClose();
	exit(0);
}
#ifdef __cplusplus
}
#endif
