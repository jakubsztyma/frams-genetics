/*
 *  conv_f8tof1_scanner.l
 *  L-systemToF1
 *
 *  Created by Maciej Wajcht on 08-03-28.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
%{
#include "lexglobal.h"
#include "conv_f8tof1_grammar.h"
#include <string.h>
#include <math.h>
	
	YYSTYPE yylval;
	int line = 1, col = 1;
	//int yyFlexLexer::yywrap(void) {
	int yywrap(void) {
		return 1;
	}
%}
%%

n[0-9]+			{ col += yyleng; 
					  yylval.strVal = yytext;
					  return PARAM_NAME; 
					}

P[0-9]+		{ col += yyleng; 
					  yylval.strVal = yytext;
					  return PROD_NAME; 
					}

-?[0-9]+(\.[0-9]+)?	{ col += yyleng;
					  //yylval.dblVal = atof(yytext);
					  yylval.strVal = yytext;
					  return DOUBLE_VAL; 
					}

[ \t\r]+				{ col += yyleng; }               /* ignore but count white space */

"="					{ col += yyleng; yylval.strVal = yytext; return ASSIGN; }

"<"					{ col += yyleng; yylval.strVal = yytext; return LESS; }
"<="				{ col += yyleng; yylval.strVal = yytext; return LESS_EQUAL; }
"=="				{ col += yyleng; yylval.strVal = yytext; return EQUAL; }
"!="				{ col += yyleng; yylval.strVal = yytext; return NOT_EQUAL; }
">="				{ col += yyleng; yylval.strVal = yytext; return GREATER_EQUAL; }
">"					{ col += yyleng; yylval.strVal = yytext; return GREATER; }

[X\[\]\^RrLlAaCcFfMmSsIiQqWwEe]		{ col += yyleng; 
					  yylval.strVal = yytext;
					  return COMMAND; 
					}

\\\[.*\\\]			{ col += yyleng;
					  yylval.strVal = yytext;
					  return NEURON;
					}

"{"					{ col += yyleng; yylval.strVal = yytext; return FORLOOP_BEGIN; }
"}"					{ col += yyleng; yylval.strVal = yytext; return FORLOOP_END; }

"("					{ col += yyleng; yylval.strVal = yytext; return LPAREN; }
")"					{ col += yyleng; yylval.strVal = yytext; return RPAREN; }

"-"{3}				{ col += yyleng; yylval.strVal = yytext; return DELIMETER; }

":"					{ col += yyleng; yylval.strVal = yytext; return SEMICOLON; }
"|"					{ col += yyleng; yylval.strVal = yytext; return PIPE; }
","					{ col += yyleng; yylval.strVal = yytext; return COMMA; }

"+"					{ col += yyleng; yylval.strVal = yytext; return PLUS; }
"-"					{ col += yyleng; yylval.strVal = yytext; return MINUS; }
"*"					{ col += yyleng; yylval.strVal = yytext; return TIMES; }
"/"					{ col += yyleng; yylval.strVal = yytext; return DIV; }

\n					{ col = 0; ++line; yylval.strVal = "\\n"; return NEWLINE; }

.					{ col += yyleng; yylval.strVal = yytext; return -1; }

%%
/**
  * reset the line and column count
  */
void reset_lexer(void)
{
	line = 1;
	col  = 1;    
}

/**
  * yyerror() is invoked when the lexer or the parser encounter
  * an error. The error message is passed via *s
  */
void yyerror(char *s)
{
	printf("error: %s at line: %d col: %d\n",s,line,col);
}

/*int yywrap(void)
{
	return 1;
}*/
