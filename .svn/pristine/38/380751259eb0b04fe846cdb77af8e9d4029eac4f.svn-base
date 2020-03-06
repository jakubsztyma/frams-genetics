// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

%{
#include "framscript-defs.h"
#include "common/log.h"
#include <math.h>
#include <ctype.h>
#include <stdio.h>

#define YYERROR_VERBOSE
#define YYPRINT(file,type,value) yyprint (file,type,value)

enum NameKind { NameNotFound, VariableName, GlobalName, ConstName };
static const char* name_kind_names[]={"","var","global","const"};

static void yyprint (FILE *file,int type,YYSTYPE value);
void handleTwoArg(YYSTYPE& result,const YYSTYPE& arg1,const YYSTYPE& arg2,
		  int optoken,const char* opname, bool negarg2, bool uniq);
bool handleCompare(YYSTYPE& result,const YYSTYPE& arg1,const YYSTYPE& arg2,
		   ExtValue::CmpOperator,const char* opname);
bool handleAssignOp(YYSTYPE& result,const YYSTYPE& var,const YYSTYPE& arg,const char* opname);
bool handleAssignOp2(YYSTYPE& result,const char *var,const YYSTYPE& arg,const char* opname,int stackpos,bool push);
bool canAddName(const SString &name,NameKind kind);
bool variableOk(TokenValue &tok, const TokenValue& var,int &loc);
int variableNameOk(const SString &name);
bool globalOk(const TokenValue& var);
bool globalNameOk(const SString& name);
void badVariable(TokenValue &tok, const TokenValue &var);
bool evalVariable(TokenValue &tok, const TokenValue &var);
bool doBreak(int level);
bool doContinue(int level);
void warnTruthValue(const TokenValue& t);
void outFunName(const TokenValue& t);
static bool resultIsRelaxedEqual(ExtValue::CompareResult res);

static const char* assign_op_names[]={"add","sub","mul","div","mod"};

%}

%token_table

%token CONSTANT
%token INVALID_NUMBER

%left ASSIGN_ADD ASSIGN_SUB ASSIGN_MUL ASSIGN_DIV ASSIGN_MOD
%right '?' ':'
%left LOGIC_OR
%left LOGIC_AND
%left '|'
%left '&'
%left EQUAL NOT_EQUAL
%left GEQUAL LEQUAL '>' '<'
%left LSHIFT RSHIFT
%left '-' '+'
%left '*' '/' '%'
%right NEG '!' PLUSPLUS MINUSMINUS
%left TYPEOF
%left INT_TYPE
%left FLOAT_TYPE
%left STRING_TYPE

%token IDENT
%token OBJNAME

%token IF      "if"
%token ELSE    "else"
%token FOR     "for"
%token INNN    "in"
%token WHILE   "while"
%token DO      "do"
%token GOTO    "goto"
%token RETURN  "return"
%token BREAK    "break"
%token CONTINUE "continue"
%token SWITCH   "switch"
%token CASE     "case"
%token DEFAULT  "default"

%token TYPEOF      "typeof"
%token INT_TYPE    "int"
%token FLOAT_TYPE  "float"
%token STRING_TYPE "string"

%token ASM     
%token ASMLINE
              
%token VAR      "var"
%token CONSTDEF "const"
%token GLOBAL   "global"
%token FUNCTION "function"

%token CALL    "call"

%token ARROW

%token ASSIGN
%token ASSIGN_ADD
%token ASSIGN_SUB
%token ASSIGN_MUL
%token ASSIGN_DIV

%token EQUAL
%token NOT_EQUAL
%token GEQUAL
%token LEQUAL

%token LOGIC_AND
%token LOGIC_OR

%token PLUSPLUS
%token MINUSMINUS

%token LSHIFT
%token RSHIFT

%%
code: {$$.setInt(trstack.currentPos());} recurcode
{
int pos=$1.getInt();
if (pos!=trstack.currentPos()) trctx.out->printf("add %d,m0\n",pos-trstack.currentPos());
}
;

recurcode:    /* empty string */
       | recurcode statement
;
     
statement: ';'
      | VAR vardeflist ';'
      | CONSTDEF constdeflist ';'
      | GLOBAL globaldeflist ';'
      | IDENT ':'     {trctx.out->printf(":%s\n",str($1));}
      | expr ';'      {if (!$1.constant) { trctx.out->printf("inc m0\n"); trstack.adjust(+1); } trctx.emitLine(); }
      | functiondef
      | blok
      | if_statement
      | goto_statement
      | return_statement
      | for_statement
      | while_statement
      | dowhile_statement
      | break_statement
      | continue_statement
      | switch_statement
//      | error ';'
      | asmblock
;

asmblock: ASM asmlines '}'
;

asmlines: /* empty */
        | ASMLINE            {trctx.out->Vputs(str($1));trctx.out->Vputc('\n');}
        | asmlines ASMLINE   {trctx.out->Vputs(str($2));trctx.out->Vputc('\n');}
;

goto_statement: GOTO IDENT ';'
 {
#ifdef FRAMSCRIPT_GOTO
trctx.out->printf("jump :%s\n",str($2)); logPrintf("FramScriptCompiler","translate",LOG_WARN,"goto is not recommended"); trctx.emitLine();
#else
trctx.err->printf("goto is not supported\n");return 1;
#endif
 }
;

return_statement: RETURN expr ';'
{
int offset;
if (trctx.functionstackpos==TranslatorContext::NOT_IN_FUNCTION)
	offset=-trstack.currentPos();
else
	offset=trctx.functionstackpos-trstack.currentPos();
if (!offset)
	{
	if ($2.constant)
		trctx.out->printf("move %s,s0\nreturn\n",litstr($2));
	else
		{
		trctx.out->printf("move m[m0++],s0\nreturn\n");
		trstack.adjust(+1);
		}
	}
else
	{
	if ($2.constant)
		{
		trctx.out->printf("add %d,m0\nmove %s,s0\nreturn\n",offset,litstr($2));
		trstack.adjust(offset);
		}
	else
		{
		trctx.out->printf("move s0,s%d\nadd %d,m0\nreturn\n",offset,offset);
		trstack.adjust(offset);
		}
	}
}
          | RETURN ';'
{
int offset;
if (trctx.functionstackpos==TranslatorContext::NOT_IN_FUNCTION)
	offset=-trstack.currentPos();
else
	offset=trctx.functionstackpos-trstack.currentPos();
trctx.emitLine();
if (!offset)
	trctx.out->printf("move invalid,s0\nreturn\n");
else
	trctx.out->printf("add %d,m0\nmove invalid,s0\nreturn\n",offset);
}
;

vardeflist: vardef
          | vardeflist ',' vardef
;

vardef: IDENT               { trctx.emitLine(); if (!canAddName($1.getString(),VariableName)) return 1; trstack.addVariable($1.getString()); trctx.out->printf("push invalid\n"); }
      | IDENT '=' stackexpr { trctx.emitLine(); if (!canAddName($1.getString(),VariableName)) return 1; trstack.adjust(1); trstack.addVariable($1.getString());}
;

constdeflist: constdef
          | constdeflist ',' constdef
;

constdef: IDENT '=' expr        { trctx.emitLine(); if (!canAddName($1.getString(),ConstName)) return 1; if (!$3.constant) {trctx.err->printf("const expression must be constant");return 1;} trstack.addConstant($1.getString(),$3); }
;

globaldeflist: globaldef
          | globaldeflist ',' globaldef
;

globaldef: IDENT     { if (!canAddName($1.getString(),GlobalName)) return 1; trstack.globals.add($1.getString(),0); trctx.out->printf("global %s\n",str($1));}
;

funparam: IDENT { trstack.addVariable($1.getString()); };

paramlist: /* empty */             {$$.setInt(0); }
         | funparam                {$$.setInt(1); }
         | paramlist ',' funparam  {$$.setInt($1.getInt()+1);}
;

funnamelist:
         IDENT {outFunName($1);}
         | funnamelist ',' IDENT {outFunName($3);}
;

functiondef:                FUNCTION funnamelist
{
trctx.emitLine();
int pos=trstack.currentPos();
$$.setInt(pos);
if (trctx.functionstackpos!=TranslatorContext::NOT_IN_FUNCTION)
	{trctx.err->printf("functions cannot be nested\n");return 1;}
trctx.beforefunctionstackpos=trstack.currentPos();
}
                            '(' paramlist ')'
{
trctx.functionstackpos=trstack.currentPos();
}
                            blok
{trctx.out->printf("move invalid,s0\nreturn\n");
int pos=$3.getInt();
trstack.dropToPos(pos);
trctx.functionstackpos=TranslatorContext::NOT_IN_FUNCTION;
trctx.beforefunctionstackpos=TranslatorContext::NOT_IN_FUNCTION;
trctx.out->printf(":_skipfun_%d\n",trctx.functiontmplabel);
trctx.functiontmplabel=-1;
trctx.emitLine();
};

break_statement: BREAK ';'         {if (!doBreak(1)) return 1;}
               | BREAK expr ';'
{
trctx.emitLine();
if (!$2.constant)
	{trctx.err->printf("break level must be a constant expression\n");return 1;}
int level=$2.getInt();
if (level<1)
	{trctx.err->printf("break level must be a positive integer\n");return 1;}
if (!doBreak(level)) return 1;
trctx.emitLine();
};

continue_statement: CONTINUE ';'         {if (!doContinue(1)) return 1;}
                  | CONTINUE expr ';'
{
if (!$2.constant)
	{trctx.err->printf("continue level must be a constant expression\n");return 1;}
int level=$2.getInt();
if (level<1)
	{trctx.err->printf("continue level must be a positive integer\n");return 1;}
if (!doContinue(level)) return 1;
trctx.emitLine();
};

while_statement: WHILE '('
{
int c=trctx.labelcounter++; $$.setInt(c);
$$.stack=trstack.currentPos();
trstack.loops.addLoop(c,$$.stack);
trctx.out->printf(":_loop_%d\n",c);}
                              expr ')'
{
int c=$3.getInt();
warnTruthValue($4);
if ($4.constant)
	{if (!$4.getInt()) trctx.out->printf("jump :_loop_end_%d\n",c);}
else
	{
	trctx.out->printf("if ~=,m[m0++],:_loop_end_%d\n",c,c);
	trstack.adjust(+1);
	}
}
                 pseudoblok_statement
{
trctx.out->printf("jump :_loop_%d\n:_loop_end_%d\n",$3.getInt(),$3.getInt());
trstack.adjust($3.stack-trstack.currentPos());
trstack.loops.drop();
}
;

dowhile_statement: DO
{
trctx.emitLine();
int c=trctx.labelcounter++; $$.setInt(c);
$$.stack=trstack.currentPos();
trstack.loops.addLoop(c,$$.stack);
trctx.out->printf(":_loop_%d\n",c);} //2

pseudoblok_statement WHILE '(' expr ')'

{//8
int c=$2.getInt();
warnTruthValue($6);
if ($6.constant)
	{if ($6.getInt()) trctx.out->printf("jump :_loop_%d\n",c);}
else
	{
	trctx.out->printf("if !~,m[m0++],:_loop_%d\n",c);
	trstack.adjust(+1);
	}
trctx.out->printf(":_loop_end_%d\n",c);
trstack.adjust($2.stack-trstack.currentPos());
trstack.loops.drop();
trctx.emitLine();
}
;

switch_statement: SWITCH '('
{
int c=trctx.labelcounter++; $1.setInt(c);
trstack.loops.addLoop(c,trstack.currentPos());}
       stackexpr ')'
{trctx.emitLine(); trctx.out->printf("dec m0\n"); trstack.adjust(-1);}
 '{' inside_switch '}'
{
trctx.emitLine(); 
LoopInfo *li=trstack.loops.getLoop(0);
trctx.out->printf(":_case_after_%d_%d\n"
		  "add 2,m0\n"
		  ":_loop_end_%d\n",
		  li->id,li->casecounter,
		  li->id);
trstack.adjust(+2);
trstack.loops.drop();
}
;

inside_switch: /* empty */
       | case_label
       | inside_switch case_label
;

case_label: CASE expr ':'
{
LoopInfo *li=trstack.loops.getLoop(0);
if ($2.constant)
	trctx.out->printf("if s1,!=,%s,:_case_before_%d_%d\n",
			  litstr($2),
			  li->id,li->casecounter+1);
else
	{
	trctx.out->printf("if s2,!=,m[m0++],:_case_before_%d_%d\n",
			  li->id,li->casecounter+1);
	trstack.adjust(+1);
	}
trctx.out->printf(":_case_after_%d_%d\n",
		  li->id,li->casecounter);
int pos=trstack.currentPos(); $$.setInt(pos);
}
 recurcode
{
trctx.emitLine();
LoopInfo *li=trstack.loops.getLoop(0);
int pos=$4.getInt();
if (pos!=trstack.currentPos()) trctx.out->printf("add %d,m0\n",pos-trstack.currentPos());
trstack.dropToPos(pos);
li->casecounter++;
trctx.out->printf("jump :_case_after_%d_%d\n"
		  ":_case_before_%d_%d\n",
		  li->id,li->casecounter,
		  li->id,li->casecounter);
}
      |  DEFAULT ':'
  {
  LoopInfo *li=trstack.loops.getLoop(0);
  trctx.out->printf(":_case_after_%d_%d\n",li->id,li->casecounter);
  }
  recurcode
  {
  LoopInfo *li=trstack.loops.getLoop(0);
  li->casecounter++;
  }
;

newvar_or_expr:
	      VAR IDENT { $$.setInt(trstack.addVariable($2.getString())); trctx.out->printf("push invalid\n"); $$.ident=true; $$.var=true; }

	      |
	      VAR IDENT '=' stackexpr
	      {
	      //trctx.out->printf("# VAR IDENT '=' stackexpr pos=%d\n",trstack.currentPos());
	      trstack.adjust(+1);
	      $$.setInt(trstack.addVariable($2.getString()));
	      $$.ident=true; $$.var=true;
	      }

	      |
	      expr_special_ident
	      {
	      $$=$1;
	      }

              | //nic
	      {
	      $$.setInt(1); $$.assign=false; $$.ident=false; $$.var=false; $$.constant=true;
	      }
;

expr_or_objname:
              expr { $$=$1; $$.objname=false; }
              |
	      OBJNAME { $$.setString($1.getString()); $$.objname=true; }
;

for_statement_begin: FOR '('
{
int c=trctx.labelcounter++; $$.counter=c; $$.stack=trstack.currentPos();
}
newvar_or_expr
{
$$=$4; $$.counter=$3.counter; $$.stack=$3.stack;
};

for_statement:

	   ///////////  for(in) ...  ////////////
           for_statement_begin INNN
	   {//3
	   if (!$1.ident)
		   {
		   trctx.err->printf("for(... in ...) requires a variable\n");
		   return 1;
		   }
	   int loc;
	   if ($1.var) // for(var x[=expr] in
		   $$.setInt($1.getInt());
	   else
		   {  // for(x in
		   if (variableOk($$,$1,loc))
			   $$.setInt(loc);
		   else if (globalOk($1))
			   {
			   trctx.err->printf("global '%s' can't be the iterating variable in 'for'\n",str($1));
			   return 1;
			   }
		   else
			   {
			   badVariable($$,$1);
			   return 1;
			   }
		   }
	   $$.stack=trstack.currentPos();
	   }
           expr_or_objname ')'
	   {//6
	   trctx.emitLine();
	   if ($4.constant) 
		   {
		   logPrintf("", "", LOG_WARN, "%s can't be iterated",str($4));
		   trctx.out->printf("jump :_loop_end_%d\n",$1.counter);
		   }
	   trstack.adjust(-1);
	   trstack.loops.addLoop($1.counter,trstack.currentPos());
	   if ($4.objname) 
		   trctx.out->printf("dec m0\nmove %s.iterator,m[m0]\n",$4.getString().c_str());
	   else
		   trctx.out->printf("move s%d,m1\ndec m0\nif ~=,m1,:_loop_end_%d\nmove [m1].\"iterator\",m[m0]\n",0,$1.counter);
	   // s0=iterator s1=obj (=obj.iterator)
	   trctx.out->printf(":_loop1_%d\n",$1.counter);
	   trctx.out->printf(":_loop_%d\n",$1.counter);
	   trctx.out->printf("move s0,m1\nmove [m1].\"next\",m2\n");
	   trctx.out->printf("if ~=,m2,:_loop_end_%d\n",$1.counter);
	   trctx.out->printf("move [m1].\"value\",s%d\n",$3.getInt()-trstack.currentPos());
	   }
	   pseudoblok_statement
	   {
	   trctx.out->printf("jump :_loop1_%d\n",$1.counter);
	   trctx.out->printf(":_loop_end_%d\n",$1.counter);
	   trstack.loops.drop();
	   if ($4.constant)
		   trstack.adjust($3.stack-trstack.currentPos());
	   int adj=$1.stack-trstack.currentPos();
	   trstack.adjust(adj);
	   if (adj!=0)
		   trctx.out->printf("add %d,m0\n",adj);
	   }
	   
|

	   ///////////  for(;;) ...  ////////////
           for_statement_begin ';'
           { //3
	   trctx.emitLine();
	   //trctx.out->printf("# for_statement_begin pos=%d ident=%d var=%d\n",trstack.currentPos(),$1.ident,$1.var);
	   if ((!$1.var) && ($1.ident))
		   {  // for(x;
		   int loc;
		   if ((!variableOk($$,$1,loc)) || (globalOk($1)))
			   {
			   badVariable($$,$1);
			   return 1;
			   }
		   }
	   if (!$1.constant && !$1.ident)
		   {
		   trctx.out->printf("inc m0\n");
		   trstack.adjust(+1);
		   }
	   trstack.loops.addLoop($1.counter,trstack.currentPos());
	   trctx.out->printf(":_loop1_%d\n",$1.counter);
	   //trctx.out->printf("# expr#2\n");
           }
           expr_or_empty ';'
	   { //6
	   trctx.emitLine();
	   int c=$1.counter;
	   warnTruthValue($4);
	   if ($4.constant)
		   {if (!$4.getInt()) trctx.out->printf("jump :_loop_end_%d\n",c);}
	   else
		   {
		   trctx.out->printf("if m[m0++],==,0,:_loop_end_%d\n",c,c);
		   trstack.adjust(+1);
		   }
	   trctx.tmp="";
	   trctx.divertOut();
	   //trctx.out->printf("# expr#3\n");
	   }
           expr_or_empty ')'
	   { //9
	   trctx.emitLine();
	   if (!$7.constant) { trctx.out->printf("inc m0\n"); trstack.adjust(+1); }
	   trctx.restoreOut();
	   $$.setString(trctx.tmp.c_str());
	   //trctx.out->printf("# pseudoblok_statement pos=%d\n",trstack.currentPos());
	   }
           pseudoblok_statement
	   {//11
	   trctx.out->printf(":_loop_%d\n",$1.counter);
	   LoopInfo* li=trstack.loops.getLoop(0);
	   if (li->location != trstack.currentPos())
		   trctx.out->printf("add %d,m0\n",li->location-trstack.currentPos());
	   trctx.out->printf(str($9));
	   if (li->location != trstack.currentPos())
		   trctx.out->printf("sub %d,m0\n",li->location-trstack.currentPos());
	   trctx.out->printf("jump :_loop1_%d\n:_loop_end_%d\n",$1.counter,$1.counter);
	   if ($1.stack != trstack.currentPos())
		   trctx.out->printf("add %d,m0\n",$1.stack-trstack.currentPos());
	   trstack.adjust($1.stack-trstack.currentPos());
	   trstack.loops.drop();
	   }
;

pseudoblok_statement:
{trctx.emitLine(); int pos=trstack.currentPos(); $$.setInt(pos);}
  statement
{
int pos=$1.getInt();
if (pos!=trstack.currentPos()) trctx.out->printf("add %d,m0\n",pos-trstack.currentPos());
trstack.dropToPos(pos);
trctx.emitLine();
};

if_statement:
 if_condition pseudoblok_statement
                       {
		       if ($1.stack!=trstack.currentPos())
			       trctx.out->printf("add %d,m0\n",$1.stack-trstack.currentPos());
		       trstack.adjust(trstack.currentPos()-$1.stack);
		       trctx.out->printf("jump :_if_end_%d\n:_if_else_%d\n",$1.getInt(),$1.getInt());
		       }
         ELSE
	               {trstack.adjust($1.stack-trstack.currentPos());}
         pseudoblok_statement
                       {
		       if ($1.stack!=trstack.currentPos())
			       trctx.out->printf("add %d,m0\n",$1.stack-trstack.currentPos());
		       trstack.adjust(trstack.currentPos()-$1.stack);
		       trctx.out->printf(":_if_end_%d\n",$1.getInt());
		       }
| 
 if_condition pseudoblok_statement
                       {
		       if ($1.stack!=trstack.currentPos())
			       trctx.out->printf("add %d,m0\n",$1.stack-trstack.currentPos());
		       trstack.dropToPos($1.stack);
		       trctx.out->printf(":_if_else_%d\n",$1.getInt());
		       }
;

if_condition: IF
{$$.stack=trstack.currentPos();trctx.emitLine();}

 '(' expr ')'
{
trctx.emitLine();
int c=trctx.labelcounter++;
$$.setInt(c);
warnTruthValue($4);
if ($4.constant)
	{
	if (!$4.getInt()) trctx.out->printf("jump :_if_else_%d\n",c);
	}
else
	{
	trctx.out->printf("if ~=,m[m0++],:_if_else_%d\n",c);
	trstack.adjust(+1);
	}
$$.stack=$2.stack;
};

blok:    '{'
{ int pos=trstack.currentPos();
$$.setInt(pos);
}
         recurcode '}'
{
int pos=$2.getInt();
if (pos!=trstack.currentPos()) trctx.out->printf("add %d,m0\n",pos-trstack.currentPos());
trstack.dropToPos(pos);
}

assign_op: ASSIGN_ADD {$$.setInt(0);}
         | ASSIGN_SUB {$$.setInt(1);}
         | ASSIGN_MUL {$$.setInt(2);}
         | ASSIGN_DIV {$$.setInt(3);}
         | ASSIGN_MOD {$$.setInt(4);}

plusminus: PLUSPLUS {$$.setInt(1);} | MINUSMINUS {$$.setInt(0);}

expr: expr_special_ident
  {
  //trctx.out->printf("# expr: ident=%d str=%s\n",$1.ident,(const char*)$1.getString());
  if ($1.ident)
	  {
	  if (evalVariable($$,$1))
		  $$.constant=false;
	  else
		  return 1;
	  }
  else
	  {$$=$1; $$.ident=false;}
  trctx.emitLine();
  }
;

stackexpr: expr {if ($1.constant) {trctx.out->printf("push %s\n",litstr($1)); trstack.adjust(-1); $$.constant=0;} }

expr_or_empty:
         expr {$$=$1;}

         | //nic
	 { $$.setInt(1); $$.assign=false; $$.constant=true; $$.ident=false; $$.ident=false; }
;

expr_special_ident:    CONSTANT             { $$=$1; $$.constant=1; $$.ident=0; }

       | IDENT                {
                              ExtValue c;
                              if (trstack.getConstant($1.getString(),c))
				      { $$=c; $$.constant=1; $$.ident=0; }
			      else
				      { $$.ident=true; $$.setString($1.getString()); }
                              }

       | OBJNAME ':' IDENT    {$$.constant=0; $$.ident=0;
                              trctx.out->printf("push %s:%s\n",$1.getString().c_str(),
						 $3.getString().c_str());
			      trstack.adjust(-1);
                              }
       | plusminus IDENT
{
trctx.emitLine();
$$.ident=0;
int loc; if (variableOk($$,$2,loc))
	{ loc-=trstack.currentPos();
	trctx.out->printf("%s s%d\npush s%d\n",$1.getInt()?"inc":"dec",loc,loc);
	trstack.adjust(-1);}
        else if (globalOk($2))
	{ trctx.out->printf("%s @%s\npush @%s\n",$1.getInt()?"inc":"dec",str($2),str($2));
	trstack.adjust(-1);}
	else {badVariable($$,$2); return 1;}
}

       | IDENT plusminus
{
trctx.emitLine();
$$.ident=0;
int loc; if (variableOk($$,$1,loc))
	{loc-=trstack.currentPos(); trctx.out->printf("push s%d\n%s s%d\n",loc,$2.getInt()?"inc":"dec",loc+1);
	trstack.adjust(-1);}
        else if (globalOk($1))
	{ trctx.out->printf("push @%s\n%s @%s\n",$1.getString().c_str(),
			    $2.getInt()?"inc":"dec",$1.getString().c_str());
	trstack.adjust(-1);}
	else {badVariable($$,$1); return 1;}
}

       | IDENT assign_op expr { trctx.emitLine(); $$.ident=0;
                                if (!handleAssignOp($$,$1,$3,assign_op_names[$2.getInt()]))
	                        if (globalOk($1)) {SString t="@"; t+=$1.getString();
                                  handleAssignOp2($$,t.c_str(),$3,assign_op_names[$2.getInt()],0,1);}
				else { badVariable($$,$1); return 1; }
                              }

       | TYPEOF '(' expr ')' { trctx.emitLine(); $$.ident=0;
                       if ($3.constant)
                             {$$.constant=1; $$=$3.getExtType();}
                       else
                             {trctx.out->printf("type s0,s0\n");}
                     }
       | INT_TYPE '(' expr ')' { trctx.emitLine(); $$.ident=0;
                       if ($3.constant)
			     {$$.constant=1; $$=ExtValue($3.getInt());}
                       else
                             {trctx.out->printf("conv 1,s0\n");}
                     }
       | FLOAT_TYPE '(' expr ')' { trctx.emitLine(); $$.ident=0;
                       if ($3.constant)
			     {$$.constant=1; $$=ExtValue($3.getDouble());}
                       else
                             {trctx.out->printf("conv 2,s0\n");}
                     }
       | STRING_TYPE '(' expr ')' { trctx.emitLine(); $$.ident=0;
                       if ($3.constant)
			     {$$.constant=1; $$=ExtValue($3.getString());}
                       else
                             {trctx.out->printf("conv 3,s0\n");}
                     }

       | expr '+' expr { handleTwoArg($$,$1,$3,'+',"add",0,1); }
       | expr '-' expr { handleTwoArg($$,$1,$3,'-',"sub",0,0); }
       | expr '*' expr { handleTwoArg($$,$1,$3,'*',"mul",0,1); }
       | expr '/' expr { handleTwoArg($$,$1,$3,'/',"div",0,0); }
       | expr '&' expr { handleTwoArg($$,$1,$3,'&',"and",0,0); }
       | expr '|' expr { handleTwoArg($$,$1,$3,'|',"or",0,0); }
       | expr '%' expr { handleTwoArg($$,$1,$3,'%',"mod",0,0); }

       | expr LOGIC_AND
 {
 // a && b:
 //   push a
 //   if (a)
 //     pop; goto and_b
 //   else
 //     goto and_end
 // and_b:
 //   push b
 // and_end:
 trctx.emitLine();
// trctx.out->printf("\n####### logic AND\n");
 int c=trctx.labelcounter++;
 $$.counter=c;
 if ($1.constant)
	 {
	 ExtValue::CompareResult cond=$1.compare(ExtValue::zero());
	 if (resultIsRelaxedEqual(cond))
		 {
		 $1.counter=0;
		 trctx.out->printf("jump :_and_end_%d\n",c);
		 }
	 else
		 $1.counter=1;
	 }
 else
	 {
	 trctx.out->printf("if !~,m[m0],:_and_b_%d\n" // skip to b if a!~=false, "a" stays on top
			   "jump :_and_end_%d\n"
			   ":_and_b_%d\n"
			   ,c,c,c);
	 }
 }
         expr
 {
 if ($1.constant)
	 {
	 if ($1.counter==0)
		 {
		 $$=$1;
		 if (!$4.constant)
			 trstack.adjust(+1);
		 }
	 else
		 $$=$4;
	 }
 else
	 {
	 $$.ident=false;
	 $$.constant=0;
	 if ($4.constant)
		 {
		 trctx.out->printf("inc m0\n"
				   "push %s\n",litstr($4));
		 }
	 else
		 {
		 trstack.adjust(+1);
		 trctx.out->printf("move m[m0],m[m0+1]\n"
				   "inc m0\n"); //drop "a"
		 }
	 }
 trctx.out->printf(":_and_end_%d\n",$3.counter);
// trctx.out->printf("#################\n\n");
 }

       | expr LOGIC_OR
 {
 // a || b:
 //   push a
 //   if (!a)
 //     pop; goto or_b
 //   else
 //     goto or_end
 // or_b:
 //   push b
 // or_end:
 trctx.emitLine();
// trctx.out->printf("\n####### logic OR\n");
 int c=trctx.labelcounter++;
 $$.counter=c;
 if ($1.constant)
	 {
	 ExtValue::CompareResult  cond=$1.compare(ExtValue::zero());
	 if (!resultIsRelaxedEqual(cond))
		 {
		 $1.counter=1;
		 trctx.out->printf("jump :_or_end_%d\n",c);
		 }
	 else
		 $1.counter=0;
	 }
 else
	 {
	 trctx.out->printf("if ~=,m[m0],:_or_b_%d\n" // skip to b if a~=false, "a" stays on top
			   "jump :_or_end_%d\n"
			   ":_or_b_%d\n"
			   ,c,c,c);
	 }
 }
         expr
 {
 if ($1.constant)
	 {
	 if ($1.counter==1)
		 {
		 $$=$1;
		 if (!$4.constant)
			 trstack.adjust(+1);
		 }
	 else
		 $$=$4;
	 }
 else
	 {
	 $$.ident=false;
	 $$.constant=0;
	 if ($4.constant)
		 {
		 trctx.out->printf("inc m0\n"
				   "push %s\n",litstr($4));
		 }
	 else
		 {
		 trstack.adjust(+1);
		 trctx.out->printf("move m[m0],m[m0+1]\n"
				   "inc m0\n"); //drop "a"
		 }
	 }
 trctx.out->printf(":_or_end_%d\n",$3.counter);
// trctx.out->printf("#################\n\n");
 }

       | expr '?'
 {
 trctx.emitLine();
// trctx.out->printf("\n####### conditional operator\n");
 $$.counter=trctx.labelcounter++;
 warnTruthValue($1);
 if ($1.constant)
	 {
	 ExtValue::CompareResult cond=$1.compare(ExtValue::zero());
	 $1.counter=0;
	 if (resultIsRelaxedEqual(cond))
		 trctx.out->printf("jump :_cond_false_%d\n",$$.counter);
	 }
 else
	 {
	 trstack.adjust(+1);
	 trctx.out->printf("if ~=,m[m0++],:_cond_false_%d\n",$$.counter);
	 }
 $$.stack=trstack.currentPos();
// trctx.out->printf("\n####### conditional - true\n");
 }
         stackexpr ':'
 {
 trctx.emitLine();
 trctx.out->printf("jump :_cond_end_%d\n",$3.counter);
 trctx.out->printf(":_cond_false_%d\n",$3.counter);
// trctx.out->printf("\n####### conditional - false\n");
 trstack.adjust($3.stack-trstack.currentPos());
 }
         stackexpr
 {
 trctx.emitLine();
 trctx.out->printf(":_cond_end_%d\n",$3.counter);
// trctx.out->printf("\n####### conditional - end\n");
 $$.ident=false; $$.constant=0; $$.parens=0; $$.assign=0;
 }
	 
       | expr LSHIFT expr { handleTwoArg($$,$1,$3,LSHIFT,"shift",0,0); }
       | expr RSHIFT expr { handleTwoArg($$,$1,$3,RSHIFT,"shift",1,0); }
       | expr EQUAL expr     { if (!handleCompare($$,$1,$3,ExtValue::CmpEQ,"==")) return 1; }
       | expr NOT_EQUAL expr { if (!handleCompare($$,$1,$3,ExtValue::CmpNE,"!=")) return 1; }
       | expr GEQUAL expr    { if (!handleCompare($$,$1,$3,ExtValue::CmpGE,">=")) return 1; }
       | expr LEQUAL expr    { if (!handleCompare($$,$1,$3,ExtValue::CmpLE,"<=")) return 1; }
       | expr '>' expr       { if (!handleCompare($$,$1,$3,ExtValue::CmpGT,">")) return 1; }
       | expr '<' expr       { if (!handleCompare($$,$1,$3,ExtValue::CmpLT,"<")) return 1; }

       | '!' expr        {
                         trctx.emitLine(); $$.assign=$2.assign; $$.parens=0; $$.ident=0;
                         if ($2.constant)
				 {$$.constant=1; ExtValue::CompareResult res=$2.compare(ExtValue((paInt)0)); $$.setInt(resultIsRelaxedEqual(res));}
                         else
				{trctx.out->printf("setif ~=,s0,s0\n");}
                         }

     | '-' expr %prec NEG { 
                          trctx.emitLine(); $$.assign=$2.assign; $$.parens=0; $$.ident=0;
                          if ($2.constant)
	                          { $$.constant=$2.constant;
				   if ($2.type==TInt) $$.setInt(-$2.getInt());
				   else if ($2.type==TDouble) $$.setDouble(-$2.getDouble());
				   else $$=$2;
				  }
			     else 
				  {
				  $$.constant=0; SString t="-"; t+=$2.getString(); $$.setString(t);
				  trctx.out->printf("mul -1,s0\n");
				  }
                          }

     | '(' expr ')'    { trctx.emitLine(); $$ = $2; $$.assign=$2.assign?(!$2.parens):0; $$.parens=1; $$.ident=0; }

     | OBJNAME '.' member {
                        trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$1.getString(); t+="."; t+=$3.getString(); $$.setString(t);
			if ($3.constant)
				{
				trctx.out->printf("push %s.%s\n",str($1),str($3)); trstack.adjust(-1);
				}
			else
				{
				trctx.out->printf("move s0,m1\nmove %s.[m1],s0\n",str($1));
				}
                        }

     | OBJNAME '.' member assign_op expr
                  { trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$1.getString(); t+="."; t+=$3.getString(); $$.setString(t);
		  if ($3.constant)
			  {
			  handleAssignOp2($$,t.c_str(),$5,assign_op_names[$4.getInt()],0,1);
			  }
		  else
			  {
			  int sp=($5.constant)?0:1;
			  t=$1.getString();t+=".[m1]";
			  trctx.out->printf("move s0,m1\n",str($1));
			  handleAssignOp2($$,t.c_str(),$5,assign_op_names[$4.getInt()],sp,0);
			  if (sp) {trctx.out->printf("inc m0\n"); trstack.adjust(1);}
			  }
                  }

     | plusminus OBJNAME '.' member {
                        trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$2.getString(); t+="."; t+=$4.getString(); $$.setString(t);
			if ($4.constant)
				{
				trctx.out->printf("%s %s.%s\npush %s.%s\n",$1.getInt()?"inc":"dec",
						  str($2),str($4),str($2),str($4));
				trstack.adjust(-1);
				}
			else
				{
				trctx.out->printf("move s0,m1\n%s %s.[m1]\nmove %s.[m1],s0\n",
						  $1.getInt()?"inc":"dec",str($2),str($2));
				}
                        }

     | OBJNAME '.' member plusminus {
                        trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$1.getString(); t+="."; t+=$3.getString(); $$.setString(t);
			if ($3.constant)
				{
				trctx.out->printf("push %s.%s\n%s %s.%s\n",
						  str($1),str($3),$4.getInt()?"inc":"dec",str($1),str($3));
				trstack.adjust(-1);
				}
			else
				{
				trctx.out->printf("move s0,m1\nmove %s.[m1],s0\n%s %s.[m1]\n",
						  str($1),$4.getInt()?"inc":"dec",str($1));
				}
                        }

     | OBJNAME '.' '*'    {
                        trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$1.getString(); t+=".*"; $$.setString(t);
			trctx.out->printf("push %s.*\n",str($1)); trstack.adjust(-1);
                        }


     | OBJNAME '.' member '=' expr {
			trctx.emitLine(); $$=$5; $$.assign=1; $$.parens=0; $$.ident=0;
			if ($3.constant)
				{
				if ($$.constant)
					trctx.out->printf("move %s,%s.%s\n",litstr($5),str($1),str($3));
				else
					trctx.out->printf("move s0,%s.%s\n",str($1),str($3));
				}
			else
				{
				if ($$.constant)
					{
					trctx.out->printf("move m[m0++],m1\nmove %s,%s.[m1]\n",
							  litstr($5),str($1));
					trstack.adjust(1);
					}
				else
					{
					trctx.out->printf("move s1,m1\nmove m[m0++],s0\nmove s0,%s.[m1]\n",
							  str($1));
					trstack.adjust(1);
					}
				}
                        }

     | OBJNAME '.' member '(' arguments ')' 
                        {
			trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$1.getString(); t+="."; t+=$3.getString(); $$.setString(t);
			int adj=0,adj2=0;
			if ($5.getInt()==0)
				{trctx.out->printf("dec m0\n");trstack.adjust(-1);adj=1;}
			if ($3.constant)
				trctx.out->printf("call %s.%s\n",str($1),str($3));
			else
				{
				trctx.out->printf("move s%d,m1\ncall %s.[m1]\n",$5.getInt()+adj,str($1));
				adj2=1;
				}
			adj2+=$5.getInt()-1+adj;
			if (adj2>0)
				{
				trctx.out->printf("add %d,m0\nxmove s%d,s0\n",adj2,-adj2);
				trstack.adjust(adj2);
				}
                        }

     | CALL expr '(' arguments ')'
             { trctx.emitLine(); $$.constant=0; $$.ident=0; $$.setString($2.getString());
	     short adj=0;
	     if ($4.getInt()==0)
		     {trctx.out->printf("dec m0\n");trstack.adjust(-1);adj=1;}
	     if ($2.constant)
		     trctx.out->printf("call %s\n",litstr($2));
	     else
		     trctx.out->printf("call s%d\n",$4.getInt()+adj);
	     if (($4.getInt()+adj) > 0)
		     {
		     trctx.out->printf("add %d,m0\nxmove s%d,s0\n",$4.getInt()+adj,-($4.getInt()+adj));
		     trstack.adjust($4.getInt()+adj);
		     }
	     }

     | FUNCTION IDENT
             { trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=":"; t+=$1.getString(); $$.setString(t);
	     trctx.out->printf("push :%s\n",$2.getString().c_str());
	     trstack.adjust(-1);
	     }

     | stackexpr '.' member
             { trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$1.getString(); t+="."; t+=$3.getString(); $$.setString(t);
	     if ($3.constant)
		     trctx.out->printf("move s0,m1\nmove [m1].%s,s0\n",str($3));
	     else
//		     trctx.out->printf("move s1,m1\nmove m[m0++],m2\nmove [m1].[m2],s0\n");
		     {trctx.out->printf("move s1,m1\nmove m[m0++],m2\nmove [m1].[m2],s0\n");trstack.adjust(1);}
	     }

     | stackexpr ARROW IDENT       /* shortcut: expr.get("ident") */
             { trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$1.getString(); t+="->"; t+=$3.getString(); $$.setString(t);
	     trctx.out->printf("move s0,m1\ncall [m1].\"get\",%s\n",litstr($3));
	     }

     | OBJNAME ARROW IDENT       /* shortcut: StaticObject.get("ident") */
             { trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$1.getString(); t+="->"; t+=$3.getString(); $$.setString(t);
	     trctx.out->printf("dec m0\ncall %s.\"get\",%s\n",$1.getString().c_str(),litstr($3)); trstack.adjust(-1);
	     }

     | plusminus stackexpr ARROW IDENT       /* shortcut: expr.set("ident",expr.get("ident")+/-1) */
             { trctx.emitLine(); $$.constant=0; $$.ident=0; $$.setString("");
	     trctx.out->printf("move s0,m1\ncall [m1].\"get\",%s\n"
			       "move s0,m2\n%s m2\n"
			       "call [m1].\"set\",%s,m2\nmove m2,s0\n",
			       litstr($4),$1.getInt()?"inc":"dec",litstr($4));
	     }

     | stackexpr ARROW IDENT plusminus       /* shortcut: expr.set("ident",expr.get("ident")+/-1) */
             { trctx.emitLine(); $$.constant=0; $$.ident=0; $$.setString("");
	     trctx.out->printf("move s0,m1\ncall [m1].\"get\",%s\n"
			       "move s0,m2\n%s s0\n"
			       "call [m1].\"set\",%s,s0\nmove m2,s0\n",
			       litstr($3),$4.getInt()?"inc":"dec",litstr($3));
	     }

     | stackexpr ARROW IDENT assign_op expr    /* shortcut: expr1.set("ident",expr1.get("ident") +*-/ expr2) */
             { trctx.emitLine(); $$.constant=0; $$.ident=0; $$.setString("");
	     if ($5.constant)
		     trctx.out->printf("move s0,m1\ncall [m1].\"get\",%s\n"
				       "move s0,m2\n%s %s,m2\n"
				       "call [m1].\"set\",%s,m2\nmove m2,s0\n",
				       litstr($3),assign_op_names[$4.getInt()],litstr($5),litstr($3));
	     else
		     {
		     trctx.out->printf("move s0,m3\nmove s1,m1\ncall [m1].\"get\",%s\n"
				       "move s0,m2\n%s m3,m2\n"
				       "call [m1].\"set\",%s,m2\ninc m0\nmove m2,s0\n",
				       litstr($3),assign_op_names[$4.getInt()],litstr($3));
		     trstack.adjust(1);
		     }
	     }

     | plusminus stackexpr '.' member
             { trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$2.getString(); t+="."; t+=$4.getString(); $$.setString(t);
	     if ($4.constant)
		     trctx.out->printf("move s0,m1\n%s [m1].%s\nmove [m1].%s,s0\n",
				       $1.getInt()?"inc":"dec",str($4),str($4));
	     else
//		     trctx.out->printf("move s1,m1\nmove m[m0++],m2\nmove [m1].[m2],s0\n");
		     {trctx.out->printf("move s1,m1\nmove m[m0++],m2\n%s [m1].[m2]\nmove [m1].[m2],s0\n",
					$1.getInt()?"inc":"dec");trstack.adjust(1);}
	     }

     | stackexpr '.' member plusminus
             { trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$1.getString(); t+="."; t+=$3.getString(); $$.setString(t);
	     if ($3.constant)
		     trctx.out->printf("move s0,m1\nmove [m1].%s,s0\n%s [m1].%s\n",
				       str($3),$4.getInt()?"inc":"dec",str($3));
	     else
//		     trctx.out->printf("move s1,m1\nmove m[m0++],m2\nmove [m1].[m2],s0\n");
		     {trctx.out->printf("move s1,m1\nmove m[m0++],m2\nmove [m1].[m2],s0\n%s [m1].[m2]\n",
					$4.getInt()?"inc":"dec");trstack.adjust(1);}
	     }

     | stackexpr '.' member assign_op expr
             { trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$1.getString(); t+="."; t+=$3.getString(); $$.setString(t);
	     if ($3.constant)
		     {
		     int sp;
		     if ($5.constant)
			     {sp=0; trctx.out->printf("move s0,m1\n");}
		     else
			     {sp=1; trctx.out->printf("move s1,m1\n");}
		     t="[m1]."; t+=str($3);
		     handleAssignOp2($$,t.c_str(),$5,assign_op_names[$4.getInt()],sp,0);
		     if (sp) {trctx.out->printf("inc m0\n");trstack.adjust(1);}
		     }
	     else
		     {
		     int sp;
		     char *t;
		     if ($5.constant)
			     {sp=1; t="move s1,m1\nmove s0,m2\n";}
		     else
			     {sp=2; t="move s2,m1\nmove s1,m2\n";}
		     trctx.out->printf(t);
		     handleAssignOp2($$,"[m1].[m2]",$5,assign_op_names[$4.getInt()],sp,0);
		     trctx.out->printf("add %d,m0\n",sp);
		     trstack.adjust(sp);
		     }
	     }

     | stackexpr '.' member '=' stackexpr
	     { trctx.emitLine(); $$=$5; $$.assign=1; $$.parens=0; $$.ident=0;
	     if ($3.constant)
		     {
		     trctx.out->printf("move s1,m1\nmove m[m0++],s0\nmove s0,[m1].%s\n",str($3));
		     trstack.adjust(1);
		     }
	     else
		     {
		     trctx.out->printf("move s2,m1\nmove s1,m2\nmove s0,[m1].[m2]\nadd 2,m0\nmove s-2,s0\n");
		     trstack.adjust(2);
		     }
	     }

     | stackexpr '.' member '(' arguments ')'
             { trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$1.getString(); t+="."; t+=$3.getString(); $$.setString(t);
	     int adj=0;
	     if ($5.getInt()==0)
		     {trctx.out->printf("dec m0\n");trstack.adjust(-1);adj=1;}
	     if ($3.constant)
		     {
		     trctx.out->printf("move s%d,m1\ncall [m1].%s\n",$5.getInt()+adj,str($3));
		     adj+=1;
		     }
	     else
		     {
		     trctx.out->printf("move s%d,m2\nmove s%d,m1\ncall [m2].[m1]\n",
				       $5.getInt()+adj+1,$5.getInt()+adj);
		     adj+=2;
		     }
	     if (($5.getInt()+adj) > 1)
		     {
		     trctx.out->printf("add %d,m0\nxmove s%d,s0\n",$5.getInt()-1+adj,-($5.getInt()-1+adj));
		     trstack.adjust($5.getInt()-1+adj);
		     }
	     }

      | stackexpr '[' expr ']' '=' expr    // shortcut: expr.set(expr,expr)
	     { trctx.emitLine(); $$=$6; $$.assign=1; $$.parens=0; $$.ident=0;
	     if ($3.constant)
		     {
		     if ($6.constant)
			     {trctx.out->printf("move s0,m1\ncall [m1].\"set\",%s,%s\ninc m0\n",litstr($3),litstr($6));trstack.adjust(+1);}
		     else
			     {trctx.out->printf("move s1,m1\npush s0\npush s0\nmove %s,s1\ncall [m1].\"set\"\nadd 3,m0\nmove s-1,s0\n",litstr($3));trstack.adjust(+1);}
		     }
	     else
		     {
		     if ($6.constant)
			     {trctx.out->printf("move s1,m1\npush %s\ncall [m1].\"set\"\nadd 3,m0\n",litstr($6)); trstack.adjust(+2);}
		     else
			     {trctx.out->printf("move s2,m1\npush s1\npush s1\ncall [m1].\"set\"\nadd 4,m0\nmove s-2,s0\n"); trstack.adjust(+2);}
		     }
	     }

      | plusminus stackexpr '[' expr ']'  /* shortcut: o.set(index,o.get(index)+/-1) */
             { trctx.emitLine(); $$.constant=0; $$.ident=0; $$.setString("");
	     if ($4.constant)
		     trctx.out->printf("move s0,m1\ncall [m1].\"get\",%s\n"
				       "move s0,m2\n%s m2\n"
				       "call [m1].\"set\",%s,m2\nmove m2,s0\n",
				       litstr($4),$1.getInt()?"inc":"dec",litstr($4));
	     else
		     {
		     trctx.out->printf("move s0,m3\nmove s1,m1\ncall [m1].\"get\",m3\n"
				       "move s0,m2\n%s m2\n"
				       "call [m1].\"set\",m3,m2\ninc m0\nmove m2,s0\n",
				       $1.getInt()?"inc":"dec");
		     trstack.adjust(1);
		     }
	     }

      | stackexpr '[' expr ']' plusminus  /* shortcut: o.set(index,o.get(index)+/-1) */
             { trctx.emitLine(); $$.constant=0; $$.ident=0; $$.setString("");
	     if ($3.constant)
		     trctx.out->printf("move s0,m1\ncall [m1].\"get\",%s\n"
				       "move s0,m2\n%s s0\n"
				       "call [m1].\"set\",%s,s0\nmove m2,s0\n",
				       litstr($3),$5.getInt()?"inc":"dec",litstr($3));
	     else
		     {
		     trctx.out->printf("move s0,m3\nmove s1,m1\ncall [m1].\"get\",m3\n"
				       "move s0,m2\n%s s0\n"
				       "call [m1].\"set\",m3,s0\ninc m0\nmove m2,s0\n",
				       $5.getInt()?"inc":"dec");
		     trstack.adjust(1);
		     }
	     }

      | stackexpr '[' expr ']' assign_op expr /* shortcut: o.set(index,o.get(index) +*-/ expr) */
             { trctx.emitLine(); $$.constant=0; $$.ident=0; $$.setString("");
	     if ($6.constant)
		     {
		     if ($3.constant)
			     trctx.out->printf("move s0,m1\ncall [m1].\"get\",%s\n"
					       "move s0,m2\n%s %s,m2\n"
					       "call [m1].\"set\",%s,m2\nmove m2,s0\n",
					       litstr($3),assign_op_names[$5.getInt()],litstr($6),litstr($3));
		     else
			     {
			     trctx.out->printf("move s0,m3\nmove s1,m1\ncall [m1].\"get\",m3\n"
					       "move s0,m2\n%s %s,m2\n"
					       "call [m1].\"set\",m3,m2\ninc m0\nmove m2,s0\n",
					       assign_op_names[$5.getInt()],litstr($6));
			     trstack.adjust(1);
			     }
		     }
	     else
		     {
		     if ($3.constant)
			     {
			     trctx.out->printf("move s0,m3\nmove s1,m1\ncall [m1].\"get\",%s\n"
					       "move s0,m2\n%s m3,m2\n"
					       "call [m1].\"set\",%s,m2\ninc m0\nmove m2,s0\n",
					       litstr($3),assign_op_names[$5.getInt()],litstr($3));
			     trstack.adjust(1);
			     }
		     else
			     {
			     trctx.out->printf("move s0,m3\nmove s1,m4\nmove s2,m1\ncall [m1].\"get\",m4\n"
					       "move s0,m2\n%s m3,m2\n"
					       "call [m1].\"set\",m4,m2\nadd 2,m0\nmove m2,s0\n",
					       assign_op_names[$5.getInt()]);
			     trstack.adjust(2);
			     }
		     }
	     }


     | stackexpr ARROW IDENT '=' expr       /* shortcut: expr.set("ident",expr) */
	     { trctx.emitLine(); $$=$5; $$.assign=1; $$.ident=0; $$.parens=0; 
		     if ($5.constant)
			     {trctx.out->printf("move s0,m1\ncall [m1].\"set\",%s,%s\ninc m0\n",litstr($3),litstr($5));$$=$5;trstack.adjust(+1);}
		     else
			     {trctx.out->printf("move s1,m1\npush s0\npush s0\nmove %s,s1\ncall [m1].\"set\"\nadd 3,m0\nmove s-1,s0\n",litstr($3));trstack.adjust(+1);}
	     }

      | stackexpr '[' expr ']'    /* shortcut: expr.get(expr) */
             { trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$1.getString(); t+=".get"; $$.setString(t);
	     if ($3.constant)
		     {
		     trctx.out->printf("move s0,m1\ncall [m1].\"get\",%s\n",litstr($3));
		     }
	     else
		     {
		     trctx.out->printf("move s1,m1\ncall [m1].\"get\"\ninc m0\nmove s-1,s0\n");
		     trstack.adjust(+1);
		     }
	     }

     | IDENT '=' expr { trctx.emitLine(); $$=$3; $$.assign=1; $$.ident=0;
                        int loc=trstack.getVariableLocation($1.getString());
			if (loc!=TranslatorStack::NOTFOUND)
			    {
			    if ($3.constant) 
			      trctx.out->printf("move %s,s%d\n",litstr($3),loc-trstack.currentPos());
			    else
			      trctx.out->printf("move s0,s%d\n",loc-trstack.currentPos());
			    }
		        else if (globalOk($1)) { $$=$3; $$.ident=0; $$.assign=1;
			  if ($3.constant) trctx.out->printf("move %s,@%s\n",litstr($3),str($1));
			  else trctx.out->printf("move s0,@%s\n",str($1));}
			else {trctx.err->printf("undefined variable: '%s'\n",str($1)); return 1;}
                      }

      | OBJNAME '[' expr ']'    /* shortcut: OBJNAME.get(expr) */
             { trctx.emitLine(); $$.constant=0; $$.ident=0; SString t=$1.getString(); t+=".get"; $$.setString(t);
	     if ($3.constant)
		     {
		     trctx.out->printf("dec m0\ncall %s.get,%s\n",str($1),litstr($3));
		     trstack.adjust(-1);
		     }
	     else
		     {
		     trctx.out->printf("call %s.get\n",str($1));
		     }
	     }

      | IDENT '(' arguments ')'
{
trctx.emitLine(); $$.constant=0; $$.ident=0; $$.setString("function call");
if ($3.getInt()==0)
	{trctx.out->printf("dec m0\n");trstack.adjust(-1);}
trctx.out->printf("call :%s\n",str($1));
if ($3.getInt()>1)
	{
	trctx.out->printf("add %d,m0\nxmove s%d,s0\n",$3.getInt()-1,-($3.getInt()-1));
	trstack.adjust($3.getInt()-1);
	}
}

| '[' {trctx.emitLine(); $$.ident=0; trctx.out->printf("add -2,m0\ncall Vector.new\nmove s0,s1\n");trstack.adjust(-2);} // s1=vector, s0=nieuzywane ale zarezerwowane zeby nie przesuwac stosu przy kazdym elemencie (trafia tu wartosc zwracana przez add/set)
        v_elements ']'
        {$$.constant=0; trctx.out->printf("inc m0\n");trstack.adjust(1);}

| '{' {trctx.emitLine(); $$.ident=0; trctx.out->printf("add -2,m0\ncall Dictionary.new\nmove s0,s1\n");trstack.adjust(-2);} // s1=dict, s0=nieuzywane ale zarezerwowane zeby nie przesuwac stosu przy kazdym elemencie (trafia tu wartosc zwracana przez add/set)
        d_elements '}'
        {$$.constant=0; trctx.out->printf("inc m0\n"); trstack.adjust(1);}

//      | '&' stackexpr {trctx.out->printf("call Ref.new\n");}

      | '&' IDENT {
        trctx.emitLine(); $$.ident=0;
	int loc=trstack.getVariableLocation($2.getString());
	if (loc!=TranslatorStack::NOTFOUND)
		{
		trctx.out->printf("push &%d\n",loc-trstack.currentPos());trstack.adjust(-1);
		}
	else if (globalOk($2))
		{
		trctx.out->printf("gpush &@%s\ncall Ref.newO\ninc m0\nmove s-1,s0\n",str($2));
		trstack.adjust(-1);
		}
	else {trctx.err->printf("undefined variable: '%s'\n",str($1)); return 1;}
            }

      | '&' OBJNAME '.' member {
      trctx.emitLine(); $$.ident=0;
      if ($4.constant)
	      {
	      trctx.out->printf("dec m0\ncall Ref.newO,%s.*,%s:%s\n",str($2),str($2),str($4));
	      trstack.adjust(-1);
	      }
      else
	      {
	      trctx.out->printf("call Ref.newO,%s.*,s0\n",str($2));
	      }
      }

      | '&' '(' stackexpr ')' '.' member {
      trctx.emitLine(); $$.ident=0;
      if ($6.constant)
	      {
	      trctx.out->printf("call Ref.newO,s0,%s\n",litstr($6));
	      }
      else
	      {
	      trctx.out->printf("call Ref.newO,s1,s0\ninc m0\nmove s-1,s0\n");
	      trstack.adjust(1);
	      }
      }

      | '(' stackexpr ',' stackexpr ',' stackexpr ')' {
      trctx.emitLine(); $$.ident=0;
      trctx.out->printf("call XYZ.new\nadd 2,m0\nmove s-2,s0\n");trstack.adjust(2);}
;

v_elements: /* empty */
      | v_element 
      | v_elements ',' v_element
;

d_elements: /* empty */
      | d_element 
      | d_elements ',' d_element
;

v_element: expr
{
if ($1.constant)
	trctx.out->printf("move s1,m1\ncall [m1].Vector:add,%s\n",litstr($1));
else
	{trctx.out->printf("move s2,m1\ncall [m1].Vector:add\ninc m0\n");trstack.adjust(1);}
}
;

d_element: expr ':' expr
{
if ($1.constant)
	{
	if ($3.constant)
		trctx.out->printf("move s1,m1\ncall [m1].Dictionary:set,%s,%s\n",litstr($1),litstr($3));
	else
		{trctx.out->printf("move s2,m1\nmove %s,s1\ncall [m1].Dictionary:set\ninc m0\n",litstr($1));trstack.adjust(1);}
	}
else
	{
	if ($3.constant)
		{trctx.out->printf("move s2,m1\nmove s0,s1\nmove %s,s0\ncall [m1].Dictionary:set\ninc m0\n",litstr($3));trstack.adjust(1);}
	else
		{trctx.out->printf("move s3,m1\ncall [m1].Dictionary:set\nadd 2,m0\n");trstack.adjust(2);}
	}
}
;

member:    IDENT { $$=$1; $$.constant=1;}
         | OBJNAME ':' IDENT { SString t=$1.getString();t+=":";t+=$3.getString();
	                       $$.setString(t);$$.constant=1;}
         | '[' stackexpr ']' { SString t="["; t+=$2.getString(); t+="]";
                               $$.setString(t); $$.constant=0;}

arguments: /* empty */         { $$.setInt(0); }
         |  stackexpr               { $$.setInt(1); }
         |  arguments ',' stackexpr  {$$.setInt($1.getInt()+1);}
;

%%

SString makeLitString(const ExtValue& val)
{
if (val.type!=TString)
	return val.getString();
SString s=val.getString();
int len=s.len();
SString ret((len*11)/10+10);
ret+='\"';
const char*t=s.c_str();
while(len>0)
	{
	switch(*t)
		{
		case '\n': ret+="\\n"; break;
		case '\r': ret+="\\r"; break;
		case '\t': ret+="\\t"; break;
		default: ret+=*t;
		}
	t++; len--;
	}
ret+='\"';
return ret;
}

static void yyprint (FILE *file,int type,YYSTYPE value)
{
fprintf(file,"(%s)%s",str(value),value.constant?"c":"");
}

int yyerror (const char *s)  /* Called by yyparse on error */
{
trctx.err->printf ("%s\n",s);
return 0; // w przykladach do bisona tez nie dali returna...
}

void handleTwoArg(YYSTYPE& result,const YYSTYPE& arg1,const YYSTYPE& arg2,
		  int optoken,const char* opname,bool negarg2,bool uniq)
{
trctx.emitLine();
result.ident=false;
if (arg1.constant && arg2.constant)
	{
	result=arg1;
	switch(optoken)
		{
		case '+': result+=arg2; break;
		case '-': result-=arg2; break;
		case '*': result*=arg2; break;
		case '/': result/=arg2; break;
		case '%': result%=arg2; break;
		case '&': result.setInt(arg1.getInt() & arg2.getInt()); break;
		case '|': result.setInt(arg1.getInt() | arg2.getInt()); break;
		case LSHIFT: result.setInt(arg1.getInt() << arg2.getInt()); break;
		case RSHIFT: result.setInt(arg1.getInt() >> arg2.getInt()); break;
		}
	}
else
	{
	//TODO: prawie kazde uzycie uniq jest niepotrzebne bo typem rzadko bedzie vector, ale w wiekszosci miejsc okreslenie typu wartosci
	// byloby bardzo trudne lub niemozliwe. mozna byloby natomiast zapamietywac przy parsowaniu czy dana wartosc na stosie jest
	// skopiowana ze zmiennej/pola czy jest wynikiem wczesniejszej operacji co pozwoliloby na likwidacje bardzo wielu uniq
	result.constant=0;
	result.assign=arg1.assign || arg2.assign;
	result.parens=0;
	result.setString(opname);
	if (arg1.constant)
		trctx.out->printf("move %s,m1\n%s%s s0,m1\nmove m1,s0\n",litstr(arg1),
				  negarg2?"neg s0\n":"",
				  opname);
	else if (arg2.constant)
		{
		if (negarg2)
			trctx.out->printf("%s %d,s0\n",opname,-arg2.getInt());
		else
			trctx.out->printf("%s%s %s,s0\n",(uniq?"uniq s0\n":""),opname,litstr(arg2));
		}
	else 
		{
		trctx.out->printf("%s%s%s s0,s1\ninc m0\n",
				  uniq?"uniq s1\n":"",
				  negarg2?"neg s0\n":"",
				  opname);
		trstack.adjust(+1);
		}
	}
}

bool handleAssignOp(YYSTYPE& result,const YYSTYPE& var,const YYSTYPE& arg,const char* opname)
{
int loc; if (variableOk(result,var,loc))
	{
	loc-=trstack.currentPos();
	if (arg.constant)
		{
		trctx.out->printf("%s %s,s%d\npush s%d\n",opname,litstr(arg),loc,loc);
		trstack.adjust(-1);
		}
	else
		trctx.out->printf("%s s0,s%d\nmove s%d,s0\n",opname,loc,loc);
	return 1;
	}
return 0;
}

bool handleAssignOp2(YYSTYPE& result,const char *var,const YYSTYPE& arg,const char* opname,int stackpos,bool push)
{
if (arg.constant)
	{
	trctx.out->printf("%s %s,%s\n",opname,litstr(arg),var);
	if (!push)
		trctx.out->printf("move %s,s%d\n",var,stackpos);
	else
		{
		trctx.out->printf("push %s\n",var);
		trstack.adjust(-1);
		}
	}
else
	trctx.out->printf("%s s0,%s\nmove %s,s%d\n",opname,var,var,stackpos);
return 1;
}

bool handleCompare(YYSTYPE& result,const YYSTYPE& arg1,const YYSTYPE& arg2,ExtValue::CmpOperator op,const char* opname)
{
trctx.emitLine();
result.ident=0;
if (arg1.constant && arg2.constant)
	{
	result.constant=1;
	ExtValue::CompareResult cmp=arg1.compare(arg2);
	ExtValue::CmpContext context;
	context.v1=&arg1;
	context.v2=&arg2;
	int ret=ExtValue::interpretCompare(op,cmp,&context);
	if (ret<0)
		result.setEmpty();//return false;
	else
		result.setInt(ret);
	return true;
	}
else
	{
	result.constant=0;
	result.assign=arg1.assign || arg2.assign;
	result.parens=0;
	result.setString(opname);
	if (arg1.constant)
		trctx.out->printf("setif %s,%s,s0,s0\n",litstr(arg1),opname);
	else if (arg2.constant)
		trctx.out->printf("setif s0,%s,%s,s0\n",opname,litstr(arg2));
	else 
		{
		trctx.out->printf("setif s1,%s,s0,s1\ninc m0\n",opname);
		trstack.adjust(+1);
		}
	return true;
	}
}

static bool resultIsRelaxedEqual(ExtValue::CompareResult res)
{
return (res==ExtValue::ResultEqual)||(res==ExtValue::ResultEqualUnordered)||(res==ExtValue::ResultUnequal_RelaxedEqual);
}

bool canAddName(const SString &name,NameKind kind)
{
ExtValue dummy;
NameKind found=NameNotFound;
if (globalNameOk(name)) found=GlobalName;
else if (trstack.getConstant(name,dummy)) found=ConstName;
else if (kind!=VariableName) { if (variableNameOk(name)!=TranslatorStack::NOTFOUND) found=VariableName; }
if (found!=NameNotFound)
	{ trctx.err->printf("Can't define '%s %s' (previously defined as %s)",name_kind_names[kind],name.c_str(),name_kind_names[found]); return false; }
return true;
}

int variableNameOk(const SString &name)
{
int loc=trstack.getVariableLocation(name);
if (loc != TranslatorStack::NOTFOUND)
	{
	if ((trctx.functionstackpos!=TranslatorContext::NOT_IN_FUNCTION)
	    && (loc>=trctx.beforefunctionstackpos))
		return TranslatorStack::NOTFOUND;
	return loc;
	}
return TranslatorStack::NOTFOUND;
}

bool variableOk(TokenValue &tok, const TokenValue& var,int &loc)
{
loc=variableNameOk(var.getString());
if (loc != TranslatorStack::NOTFOUND)
	{
	tok.setInt(loc); tok.constant=0;
	return true;
	}
return false;
}

bool globalOk(const TokenValue& var)
{
return globalNameOk(var.getString());
}

bool globalNameOk(const SString& name)
{
SymTabEntry* found=trstack.globals.find(name);
if (found) return true;
return framscriptIsGlobalName(name.c_str());
}

void badVariable(TokenValue &tok, const TokenValue& var)
{
tok=var; tok.constant=1;
trctx.err->printf("undefined variable '%s'\n",str(var));
}

bool doBreak(int level)
{
if (trstack.loops.size()<level)
	{trctx.err->printf("invalid 'break'\n"); return 0;}
LoopInfo* li=trstack.loops.getLoop(level-1);
if (li->location != trstack.currentPos())
	trctx.out->printf("add %d,m0\n",li->location-trstack.currentPos());
trctx.out->printf("jump :_loop_end_%d\n",li->id);
return 1;
}

bool doContinue(int level)
{
if (trstack.loops.size()<level)
	{trctx.err->printf("invalid 'continue'\n"); return 0;}
LoopInfo* li=trstack.loops.getLoop(level-1);
if (li->location != trstack.currentPos())
	trctx.out->printf("add %d,m0\n",li->location-trstack.currentPos());
trctx.out->printf("jump :_loop_%d\n",li->id);
return 1;
}

int lookupToken(char *s)
{
int len=strlen(s);
int i;
const char *t;
for (i = 0; i < YYNTOKENS; i++)
	{
	t=yytname[i];
	if (t && (t[0]=='"') 
	   && (!strncmp(t+1,s,len))
	   && (t[len+1]=='"') 
	   && (t[len+2] == 0))
		return yytoknum[i];
	}
return -1;
}

void warnTruthValue(const TokenValue& t)
{
if (t.assign && (!t.parens))
	logPrintf("FramScriptCompiler","translate",LOG_WARN,"Assignment used as truth value, use ((double parens)) if you really mean it");
}

void outFunName(const TokenValue& t)
{
if (trctx.functiontmplabel<0)
	{
	trctx.functiontmplabel=trctx.labelcounter++;
	trctx.out->printf("jump :_skipfun_%d\n",trctx.functiontmplabel);
	}
trctx.out->printf(":%s\n",str(t));
}

bool evalVariable(TokenValue &tok,const TokenValue &var)
{
int loc;
if (variableOk(tok,var,loc))
	{
	trctx.out->printf("push s%d\n",loc-trstack.currentPos());
	trstack.adjust(-1);
	return true;
	}
else if (globalOk(var))
	{
	trctx.out->printf("push @%s\n",var.getString().c_str());
	trstack.adjust(-1);
	return true;
	}
else
	{
	badVariable(tok,var); return false;
	}
}
