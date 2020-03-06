%{
// This file is a part of the Framsticks SDK.
// Copyright (C) 2002-2015  Maciej Komosinski and Szymon Ulatowski.  See LICENSE.txt for details.
// Refer to http://www.framsticks.com/ for further information.

#include "framscript-defs.h"
#include "framscript.tab.cpp.h"
#include <math.h>

#define YY_INPUT(buf,result,maxsize) {result=trctx.in->Vread(buf,1,maxsize);}
#define YY_NEVER_INTERACTIVE 1

static SString quoteMultiline(const SString &src);

%}

%option noyywrap
%x asmcode
%x asmcode2
%x comment
%x multiline

%%

[0-9]+\.[0-9]*                   |
[0-9]+\.[0-9]*[eE][+-]?[0-9]+    |
[0-9]+*[eE][+-]?[0-9]+           {if (framscriptlval.parseNumber(yytext,TDouble)==(yytext+yyleng)) return CONSTANT; else { trctx.err->printf("Invalid floating point number: '%s', ",SString(yytext,yyleng).c_str()); return INVALID_NUMBER;} }

[0-9]+                |
0x[0-9a-fA-F]+        {if (framscriptlval.parseNumber(yytext,TInt)==(yytext+yyleng)) return CONSTANT; else { trctx.err->printf("Invalid integer: '%s', ",SString(yytext,yyleng).c_str()); return INVALID_NUMBER;} }

"null"            {framscriptlval.setEmpty(); return CONSTANT;}
\"([^\\\"]|\\.)*\" {framscriptlval.setString(SString(yytext+1,yyleng-2));return CONSTANT;}
\"\"\"             {trctx.tmp=""; trctx.multilimit='\"'; BEGIN multiline;}
"'''"              {trctx.tmp=""; trctx.multilimit='\''; BEGIN multiline;}
"@line "[0-9]+\n   {trctx.line=atol(yytext+6);trctx.linechanged=true;}
"@file "[^\n\t\r]+\n {trctx.srcname=SString(yytext+6,yyleng-7);trctx.namechanged=true;}

[a-zA-Z_][a-zA-Z0-9_]* { int t=lookupToken(yytext);
                         if (t>=0) return t;
                         else
                            {
                            framscriptlval.setString(yytext);
                            return framscriptIsObjectName(yytext)?OBJNAME:IDENT;
                            }
                       }

"->"               return ARROW;
"=="               return EQUAL;
"<>"               return NOT_EQUAL;
"!="               return NOT_EQUAL;
">="               return GEQUAL;
"<="               return LEQUAL;

"+="                return ASSIGN_ADD;
"-="                return ASSIGN_SUB;
"*="                return ASSIGN_MUL;
"/="                return ASSIGN_DIV;
"%="                return ASSIGN_MOD;

"++"                return PLUSPLUS;
"--"                return MINUSMINUS;

"&&"                return LOGIC_AND;
"||"                return LOGIC_OR;

"<<"                return LSHIFT;
">>"                return RSHIFT;

<INITIAL,asmcode,asmcode2>\/\/.*\n         {trctx.line++;trctx.linechanged=true;} // komentarz jednoliniowy
<INITIAL,asmcode,asmcode2>\/\/[^\n]*       ;               // komentarz ale nie ma potem \n

<INITIAL>\/\*       BEGIN comment;
<comment>\n         {trctx.line++;trctx.linechanged=true;}
<comment>\*\/       BEGIN 0;
<comment>.          ;
<multiline>.*       {
                    char* end=strstr(yytext,trctx.multilimit=='\"' ? "\"\"\"" : "'''");
                    if (end)
                       {
                       trctx.tmp+=string(yytext,end-yytext);
                       framscriptlval.setString(quoteMultiline(SString(trctx.tmp.c_str())));
                       yyless((end-yytext)+3);
                       BEGIN 0;
                       return CONSTANT;
                       }
                    else
                       trctx.tmp+=string(yytext,yyleng);
                    }
<multiline>\n       {trctx.line++; trctx.linechanged=true; trctx.tmp+="\n";}
[ \t\r\f]           ;
\n                  {trctx.line++; trctx.linechanged=true;}
.                   return (int)(unsigned char)yytext[0];

"asm"[ \t\n\r]*"{"  {
                    BEGIN asmcode;
                    char *t=yytext;
                    while(t=strchr(t+1,'\n')) trctx.line++;
                    trctx.linechanged=true;
                    return ASM;
                    }
<asmcode>.*\n {
              char *t=yytext;
              trctx.line++; trctx.linechanged=true;
              while ((*t==' ')||(*t=='\t')) t++;
              if (*t=='}') {yyless((t-yytext)+1); BEGIN 0; return '}';}
              char *e=yytext+yyleng-1;
              while (e[-1]=='\r') e--;
              framscriptlval.setString(SString(t,e-t));
              return ASMLINE;
              }

%%

//exclusively for multiline, but it should not be needed, there is some unexpected difference between quoting \n and \" in framscript parsing (TODO)
static SString quoteMultiline(const SString &src)
{
int len=src.len();
SString ret((len*11)/10+10);
const char*t=src.c_str();
while(len>0)
	{
	switch(*t)
            {
            case '\"': ret+="\\\""; break;
            case '\\': ret+="\\\\"; break;
            default: ret+=*t;
            }
	t++; len--;
	}
return ret;
}

void framscript_init_lex()
{
BEGIN 0;
yyrestart(0);
}

void framscript_cleanup_lex()
{
yy_delete_buffer(yy_current_buffer);
}
