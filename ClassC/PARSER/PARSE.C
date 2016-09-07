/*             %M%    %I%  %H%  %G%                */
/*
 *		ClassC -- Object Oriented C Language
 *	Copyright (c) 1988 by Paul Kirkaas
 *
 *	This program may be freely used and/or modified, 
 *	with the following provisions:
 *	1. This notice and the above copyright notice must remain intact.
 *	2. Neither this program, nor any modification of it,
 *	   may be sold for profit without written consent of the author.
 *
 */
#include <fcntl.h>
#include <stdio.h>
#include "y.tab.h"
#include "common.h"
/*   Global definitions   */
FILE *	outFP,inFP;
int	nout = 0; /* No output flag  --  set on detection of error */
extern  char * prep;
extern  int   debugQ;
/*****************************************************************************/
extern  FILE * yyout, * yyin;
main(argc,argv) int argc; char * argv[];
{ 
 char outfile [99];
 char tumpfile [99];
 char * cppopts = stral(" ");
 extern int debug;
#ifdef ATT
 extern int yydebug;
#endif
 char cmd[99];
 int tmpi;
 int i,j,k;
 int pid = getpid();
 extern int garbQ; /* defined in y1 -- if = 1, compile for garbage collection*/
 free (malloc (1000000));
 pkdbg = 0;
 fflush(stdout);
/* Interpret command line arguments */
 for(i=2; i<argc; i++)
 {
     if (!strcmp (argv[i],"-nogarb")) garbQ = 0;
     else if (!strcmp(argv[i],"-garb")) garbQ = 1;
     else if (!strcmp (argv[i],"1")) pkdbg = 1;
#ifdef ATT
     else if (!strcmp (argv[i],"2")) {pkdbg = 2; yydebug = 1;}
#endif
     else if (!strcmp (argv[i],"-funcderef")) derefQ=1;
     else if (!strcmp (argv[i],"-inlinederef")) derefQ=0;
     else if (!strcmp (argv[i],"-debug")) debugQ=1;
     else if (!strncmp (argv[i],"-D",2)) cppopts=
		vwrite(vwrite(cppopts," "),argv[i]);
     else fprintf (stderr,"Error in ceparser options: <%s> unknown\n",argv[i]);
 }
/*
 printf("CPP OPTS: %s\n",cppopts);
*/
 if ( (tmpi = open (argv[1], O_RDONLY)) == -1 )
     {fprintf(stderr,"File %s does not exist\n",argv[1]);
      exit (1);
     }
 close (tmpi);
 sprintf (tumpfile,"/tmp/clce%d",pid);
 strcpy(outfile,argv[1]);
 if (outfile[tmpi = strlen(outfile)-1] == 'e') outfile[tmpi] = 'c';
 else {fprintf(stderr,"Error: ClassC/Elaine source file must end in 'e'\n");
	exit(1); }
 sprintf(cmd,"/lib/cpp %s %s > %s",cppopts,argv[1], tumpfile);
 system (cmd);
 yyin = fopen(tumpfile,"r");
 outFP = fopen(outfile,"w");
 classList = buildClassList(classList);
 rewind(yyin);
 yyparse();
 if (nout) exit(1);
 fclose (outFP);
 sprintf (cmd,"rm %s",tumpfile);
 system(cmd);
 sprintf(cmd,"/bin/cp %s /tmp/clcb%d",outfile,pid);
/*
 sprintf(cmd,"/usr/bin/cb %s > /tmp/clcb%d",outfile,pid);
*/
 system (cmd);
 sprintf(cmd,"mv /tmp/clcb%d %s",pid,outfile);
 system (cmd);
 exit(0);
}
