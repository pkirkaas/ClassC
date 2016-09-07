%{
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

#include "common.h"
#include "y.tab.h"
extern YYSTYPE yylval;
/* SET -- sets yylval to a dynamic string equal to yytext */
#define SET yylval.t = 0; yylval.r = ztwrite(0,yytext," ",0);

/* COMMENT DETECTING STRING --
\/\*([^\*]*|\*+[^\*\/])*\*+\/
*/

%}
RAT ([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+)
dlm [ \t\n]
ws	{dlm}+
%%
"#"[ \t]*[0-9]+.*\n	{/* This is a preprocessor line declaration */
			    setcurrline(yytext);
			    *yytext = 0;
			    /* Take out the overly vexing debug lines 
			    lostlines = vwrite(lostlines,yytext);
			    return yylex();
			    */
			}
"#".*\n			{ /* Any other preprocessor statement -- just increment
			   * line count */
			     currline++;
			}
"//".*\n		{ /* Inline comments -- like C++ -- just increment
			   * line count */
			     currline++;
			}

\n				{/*  This just increments the line count */
				  currline++;
				  *yytext = 0;
				}
[ \t]*				{/*  This just picks up whitespace */
				  *yytext = 0;
				}
\"([^\\"\n]|(\\\")|(\\[^"]))*\"		{ SET; return STR;}
\'([^\\'\n]|(\\\')|(\\[^'\n]))+\'	{SET; return CHARACTER; }
{RAT}|({RAT}|[0-9]+)((e|E)(\+|-)?[0-9]+)    {/*printf("Float %s\n",yytext);*/
					      SET; return REAL;}

((0[xX][0-9a-fA-F]+)|([0-9]+))[lL]?	{ SET; return INTEGER;}
\'([^\']|\\\')+\'			{ SET; return INTEGER;}
"::"	{SET; return SCOPER;   /* Kustom Klass Operators */}
":>"	{SET; return CLREF;}
new	{SET; return NEW; }

"."	{SET; return '.';}
"->"	{SET; return SP;}

auto	{SET; return STORE;}   
static	{SET; return STORE;}
extern	{SET; return STORE;}
register	{SET; return STORE;}
typedef	{SET; return TYPEDEF;}

object 	{SET; return OBJ;}
Object 	{SET; return OBJ;}
void 	{SET; return VOID;}
char	{SET; return CHAR;}
float	{SET; return FLOAT;}
double	{SET; return FLOAT;}
int	{SET; return INT;}
short{dlm}+(int{dlm}+)?		{SET; return SHORT;}
long{dlm}+(int{dlm}+)?		{SET; return LONG;}
unsigned{dlm}+(int{dlm}+)?	{SET; return UNSGN;}

class	{SET; return CLASS;}
struct	{SET; return STRUCT;}
union	{SET; return UNION;}
enum	{SET; return ENUM;}

if 	{SET; return IF;}
while	{SET; return WHILE;}
for	{SET; return FOR;}
do		{SET; return DO;}
else	{SET; return ELSE;}
switch	{SET; return SWITCH;}
break	{SET; return BREAK	;}
continue	{SET; return CONTINUE;}
return	{SET; return RETURN;}
goto	{SET; return GOTO;}
case	{SET; return CASE;}
default	{SET; return DEFAULT;}
sizeof	{SET; return SIZEOF;}
";"	{SET; return ';';}
"}"	{SET; return '}';}
"{"	{SET; return '{';}
"("	{SET; return '(';}
")"	{SET; return ')';}
"]"	{SET; return ']';}
"["	{SET; return '[';}

":"	{SET; return ':';	/* Dual - function operators */}
"*"	{SET; return '*';}
"&"	{SET; return '&';}
"-"	{SET; return '-';}

"++"	{SET; return DINC;	/* Unary only operators */}
"--"	{SET; return DINC;}
"!"	{SET; return '!';}
"~"	{SET; return '~';}

"?"	{SET; return '?';	/* binary operators */}
","	{SET; return ',';}
"/"	{SET; return '/';}
"%"	{SET; return '%';}
"+"	{SET; return '+';}
"^"	{SET; return '^';}
"|"	{SET; return '|';}
"<<"	{SET; return LSH;}
">>"	{SET; return RSH;}
"<"	{SET; return '<';}
">"	{SET; return '>';}
"<="	{SET; return LEQ;}
">="	{SET; return GEQ;}
"=="	{SET; return EQ;}
"!="	{SET; return NEQ;}
"&&"	{SET; return AND;}
"||"	{SET; return OR;}

"="	{SET; return '=';}
"+="	{SET; return ASSIGN;}
"-="	{SET; return ASSIGN;}
"*="	{SET; return ASSIGN;}
"/="	{SET; return ASSIGN;}
"%="	{SET; return ASSIGN;}
"&="	{SET; return ASSIGN;}
"|="	{SET; return ASSIGN;}
"^="	{SET; return ASSIGN;}
"<<="	{SET; return ASSIGN;}
">>="	{SET; return ASSIGN;}

[_a-zA-Z][0-9_a-zA-Z]*{dlm}*::	{ SET; return NAME_SCOPER; /* UGLY kludge */}
[_a-zA-Z][0-9_a-zA-Z]* { SET; 
			      /* if ident has been predefined by a typedef -- */
			      if (find(yytext,Ttable)) return SYNTH;
			      /* if ident has been predefined by a CLASSdef --*/
			      if (find(yytext,Ktable)) return KNAME;
			      if ( inList(yytext,classList)) return KNAME;
			      /* otherwise, is a normal identifier */
			      else return IDENT;
			     }
%%
/*
*/
/*Sets line number & current file name from string "# <lineno> [<filename>]"*/
setcurrline(str)  char* str;
{ static char tststr[99] = "";
  sscanf(str," # %d \"%[^\"]",&currline,tststr);
  if (*tststr) strcpy (currfile,tststr);
}

/* Counts the number of newlines in a comment string */
lineinc(str) char * str;
{
 char * ind = str;
 for (ind = str; ind < str + strlen(str); ind++) if (*ind == '\n') currline++;
}
