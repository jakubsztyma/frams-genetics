/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is include which follows the "include" declaration
** in the input file. */
#include <stdio.h>
#include <assert.h>
#line 1 "conv_f8tof1_grammar.y"

#include <iostream>
#include <sstream>
#include "lemonglobal.h"
#include "conv_f8tof1.h"
#include "conv_f8_utils.h"
#define PARSER_DEBUG 0
#line 18 "conv_f8tof1_grammar.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    ParseTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 51
#define YYACTIONTYPE unsigned char
#define ParseTOKENTYPE Token
typedef union {
  ParseTOKENTYPE yy0;
  ProductionTailToken yy11;
  Token yy78;
  ProdPieceToken yy89;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL
#define ParseARG_PDECL
#define ParseARG_FETCH
#define ParseARG_STORE
#define YYNSTATE 91
#define YYNRULE 47
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* Next are that tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    46,    4,   23,   24,   26,   27,   22,   28,   85,   61,
 /*    10 */    31,   62,   43,   56,    4,   40,   37,   83,   76,   66,
 /*    20 */    63,   69,   71,   31,   72,   43,    9,    4,   40,   37,
 /*    30 */    83,   76,   47,    4,   69,   71,   70,   14,   43,  139,
 /*    40 */    17,    4,   70,   59,   43,   51,   66,   69,   71,   57,
 /*    50 */    70,   52,   43,   69,   71,   38,    4,   58,   15,   11,
 /*    60 */    84,   69,   71,   19,   61,   70,   62,   43,   12,   13,
 /*    70 */    15,   11,   44,   89,   30,   34,   69,   71,   16,   48,
 /*    80 */    75,    5,   76,   91,   66,   72,   73,   74,   12,   13,
 /*    90 */    15,   11,   61,   20,   62,   65,   66,    6,    1,    2,
 /*   100 */    65,    7,   42,   33,   35,   54,   10,   39,   41,   10,
 /*   110 */    39,   41,   61,   49,   62,   68,   61,   61,   62,   62,
 /*   120 */    61,   88,   62,   33,   67,   65,   66,   36,   32,   25,
 /*   130 */     8,   64,   52,   18,   16,   48,   86,   55,   29,   53,
 /*   140 */    60,   87,   90,   81,    3,   77,   78,   45,   21,   79,
 /*   150 */   140,  140,   80,   82,   50,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */    27,   28,   13,   14,   15,   16,   17,   18,   11,   35,
 /*    10 */    37,   37,   39,   27,   28,   42,   43,   44,   45,   22,
 /*    20 */    46,   48,   49,   37,   23,   39,   33,   28,   42,   43,
 /*    30 */    44,   45,   39,   28,   48,   49,   37,   10,   39,   29,
 /*    40 */    30,   28,   37,   44,   39,   35,   22,   48,   49,   44,
 /*    50 */    37,   37,   39,   48,   49,   41,   28,   44,    4,    5,
 /*    60 */    11,   48,   49,   10,   35,   37,   37,   39,    2,    3,
 /*    70 */     4,    5,   44,   31,   37,   46,   48,   49,   36,   37,
 /*    80 */    43,   19,   45,    0,   22,   23,   24,   25,    2,    3,
 /*    90 */     4,    5,   35,    1,   37,   21,   22,   11,    9,    9,
 /*   100 */    21,   10,   34,   46,   47,   34,   38,   39,   40,   38,
 /*   110 */    39,   40,   35,   20,   37,   11,   35,   35,   37,   37,
 /*   120 */    35,    6,   37,   46,   47,   21,   22,   46,   46,   31,
 /*   130 */     1,   46,   37,    1,   36,   37,   41,    6,    8,    6,
 /*   140 */    11,    6,    6,   35,   12,   35,   35,    7,   32,   35,
 /*   150 */    50,   50,   35,   35,   35,
};
#define YY_SHIFT_USE_DFLT (-12)
#define YY_SHIFT_MAX 52
static const short yy_shift_ofst[] = {
 /*     0 */    79,   62,   62,   62,   62,   62,   62,  104,   74,    1,
 /*    10 */     1,   74,   74,   74,   74,   74,   24,   24,   24,   -3,
 /*    20 */    24,    1,   79,   79,   79,  140,   79,   79,   79,   79,
 /*    30 */   -11,  -11,   86,   66,   54,  129,   54,  132,   49,   53,
 /*    40 */    89,   90,   83,   91,   93,  115,  131,  135,  130,   27,
 /*    50 */   136,  133,   92,
};
#define YY_REDUCE_USE_DFLT (-28)
#define YY_REDUCE_MAX 29
static const signed char yy_reduce_ofst[] = {
 /*     0 */    10,  -14,  -27,    5,   13,   28,   -1,   57,   77,   68,
 /*    10 */    71,  -26,   29,   81,   82,   85,   42,   98,   37,   14,
 /*    20 */    95,   -7,  108,  110,  111,  116,  114,  117,  118,  119,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   138,  117,  117,  117,  117,  117,  117,  138,  138,   98,
 /*    10 */    98,  138,  138,  138,  138,  138,   95,   95,  138,  138,
 /*    20 */   138,  138,  138,  138,  138,  138,  138,  138,  138,  138,
 /*    30 */   138,  123,  138,  125,  129,  138,  130,  138,  138,  138,
 /*    40 */   106,  138,  138,  138,  138,  138,  138,  138,  138,  138,
 /*    50 */   138,  138,  104,   92,   97,  100,  105,  107,  118,  119,
 /*    60 */   120,  127,  128,  131,  132,  133,  134,  126,  121,  122,
 /*    70 */   123,  124,  135,  136,  137,  109,  110,  111,  112,  113,
 /*    80 */   114,  115,  116,  108,  101,  102,  103,   99,   93,   94,
 /*    90 */    96,
};
#define YY_SZ_ACTTAB (int)(sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammer, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  int stateno;       /* The state-number */
  int major;         /* The major token value.  This is the code
                     ** number for the token at this stack level */
  YYMINORTYPE minor; /* The user-supplied minor token value.  This
                     ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
  int yyerrcnt;                 /* Shifts left before out of the error */
  ParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "COMMA",         "PLUS",          "MINUS",       
  "DIV",           "TIMES",         "NEWLINE",       "DELIMETER",   
  "ASSIGN",        "SEMICOLON",     "LPAREN",        "RPAREN",      
  "PIPE",          "LESS",          "LESS_EQUAL",    "NOT_EQUAL",   
  "EQUAL",         "GREATER_EQUAL",  "GREATER",       "FORLOOP_BEGIN",
  "FORLOOP_END",   "DOUBLE_VAL",    "PARAM_NAME",    "PROD_NAME",   
  "COMMAND",       "NEURON",        "error",         "production_tail",
  "prod_piece",    "program",       "counter",       "start_params",
  "delim",         "first_prod",    "productions",   "double_val",  
  "start_parameter",  "param_name",    "production",    "prod_name",   
  "production_head",  "prod_params",   "subproduction",  "conditions",  
  "real_prod",     "condition",     "single_val",    "multiple_val",
  "command",       "neuron",      
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "program ::= counter start_params delim first_prod productions",
 /*   1 */ "counter ::= double_val NEWLINE",
 /*   2 */ "delim ::= DELIMETER NEWLINE",
 /*   3 */ "start_params ::= start_parameter start_params",
 /*   4 */ "start_params ::=",
 /*   5 */ "start_parameter ::= param_name ASSIGN double_val NEWLINE",
 /*   6 */ "productions ::= production productions",
 /*   7 */ "productions ::=",
 /*   8 */ "first_prod ::= prod_name NEWLINE",
 /*   9 */ "production ::= production_head SEMICOLON production_tail NEWLINE",
 /*  10 */ "production_head ::= prod_name LPAREN prod_params RPAREN",
 /*  11 */ "production_head ::= prod_name LPAREN RPAREN",
 /*  12 */ "prod_params ::= param_name COMMA prod_params",
 /*  13 */ "prod_params ::= param_name",
 /*  14 */ "production_tail ::= subproduction SEMICOLON production_tail",
 /*  15 */ "production_tail ::= subproduction",
 /*  16 */ "subproduction ::= conditions PIPE real_prod",
 /*  17 */ "subproduction ::= real_prod",
 /*  18 */ "conditions ::= conditions COMMA conditions",
 /*  19 */ "conditions ::= condition",
 /*  20 */ "condition ::= param_name LESS double_val",
 /*  21 */ "condition ::= param_name LESS_EQUAL double_val",
 /*  22 */ "condition ::= param_name NOT_EQUAL double_val",
 /*  23 */ "condition ::= param_name EQUAL double_val",
 /*  24 */ "condition ::= param_name GREATER_EQUAL double_val",
 /*  25 */ "condition ::= param_name GREATER double_val",
 /*  26 */ "real_prod ::=",
 /*  27 */ "real_prod ::= prod_piece real_prod",
 /*  28 */ "real_prod ::= FORLOOP_BEGIN real_prod FORLOOP_END LPAREN single_val RPAREN real_prod",
 /*  29 */ "prod_piece ::= prod_name LPAREN multiple_val RPAREN",
 /*  30 */ "prod_piece ::= prod_name LPAREN RPAREN",
 /*  31 */ "prod_piece ::= command",
 /*  32 */ "prod_piece ::= param_name",
 /*  33 */ "prod_piece ::= neuron",
 /*  34 */ "multiple_val ::= single_val",
 /*  35 */ "multiple_val ::= multiple_val COMMA multiple_val",
 /*  36 */ "single_val ::= double_val",
 /*  37 */ "single_val ::= param_name",
 /*  38 */ "single_val ::= single_val PLUS single_val",
 /*  39 */ "single_val ::= single_val MINUS single_val",
 /*  40 */ "single_val ::= single_val TIMES single_val",
 /*  41 */ "single_val ::= single_val DIV single_val",
 /*  42 */ "double_val ::= DOUBLE_VAL",
 /*  43 */ "param_name ::= PARAM_NAME",
 /*  44 */ "prod_name ::= PROD_NAME",
 /*  45 */ "command ::= COMMAND",
 /*  46 */ "neuron ::= NEURON",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/* MW */
Lsystem *lsys;
bool syntaxOnly;
bool *syntaxOk;

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and ParseFree.
*/
void *ParseAlloc(void *(*mallocProc)(size_t), Lsystem *lsystem, bool checkSyntaxOnly = false, bool *isSyntaxOk = NULL){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#if YYSTACKDEPTH<=0
    yyGrowStack(pParser);
#endif
	  //input = new Lsystem();
	  lsys = lsystem;
	  syntaxOnly = checkSyntaxOnly;
	  syntaxOk = isSyntaxOk;
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(YYCODETYPE yymajor, YYMINORTYPE *yypminor){
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor( yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from ParseAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
	//delete lsys;
}

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  if( stateno>YY_SHIFT_MAX || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
#ifdef YYFALLBACK
      int iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( j>=0 && j<YY_SZ_ACTTAB && yy_lookahead[j]==YYWILDCARD ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  assert( stateno<=YY_REDUCE_MAX );
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  assert( i>=0 && i<YY_SZ_ACTTAB );
  assert( yy_lookahead[i]==iLookAhead );
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   ParseARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
   ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer ot the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#if YYSTACKDEPTH>0 
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = yyNewState;
  yytos->major = yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 29, 5 },
  { 30, 2 },
  { 32, 2 },
  { 31, 2 },
  { 31, 0 },
  { 36, 4 },
  { 34, 2 },
  { 34, 0 },
  { 33, 2 },
  { 38, 4 },
  { 40, 4 },
  { 40, 3 },
  { 41, 3 },
  { 41, 1 },
  { 27, 3 },
  { 27, 1 },
  { 42, 3 },
  { 42, 1 },
  { 43, 3 },
  { 43, 1 },
  { 45, 3 },
  { 45, 3 },
  { 45, 3 },
  { 45, 3 },
  { 45, 3 },
  { 45, 3 },
  { 44, 0 },
  { 44, 2 },
  { 44, 7 },
  { 28, 4 },
  { 28, 3 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 47, 1 },
  { 47, 3 },
  { 46, 1 },
  { 46, 1 },
  { 46, 3 },
  { 46, 3 },
  { 46, 3 },
  { 46, 3 },
  { 35, 1 },
  { 37, 1 },
  { 39, 1 },
  { 48, 1 },
  { 49, 1 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.  
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  memset(&yygotominor, 0, sizeof(yygotominor));


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0: /* program ::= counter start_params delim first_prod productions */
#line 18 "conv_f8tof1_grammar.y"
{
	*syntaxOk = true;	
}
#line 741 "conv_f8tof1_grammar.c"
        break;
      case 1: /* counter ::= double_val NEWLINE */
#line 22 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "counter ::= double_val NEWLINE." << std::endl;
#endif
		}
		lsys->iterations = (int) yymsp[-1].minor.yy78.dblValue; 
	}
}
#line 755 "conv_f8tof1_grammar.c"
        break;
      case 2: /* delim ::= DELIMETER NEWLINE */
      case 3: /* start_params ::= start_parameter start_params */
      case 4: /* start_params ::= */
      case 6: /* productions ::= production productions */
      case 7: /* productions ::= */
#line 33 "conv_f8tof1_grammar.y"
{
}
#line 765 "conv_f8tof1_grammar.c"
        break;
      case 5: /* start_parameter ::= param_name ASSIGN double_val NEWLINE */
#line 38 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{	
#if PARSER_DEBUG > 0
			std::cout << "start_parameter ::= param_name ASSIGN double_val NEWLINE." << std::endl;
#endif
		}
		lsys->startParams[sstringToString(*(yymsp[-3].minor.yy78.strValue))] = yymsp[-1].minor.yy78.dblValue;
#if PARSER_DEBUG > 0
		cout << "**** " << lsys->startParams["n0"] << endl;
		cout << "**** " << lsys->startParams["n1"] << endl;
#endif
		delete yymsp[-3].minor.yy78.strValue;
	}
}
#line 784 "conv_f8tof1_grammar.c"
        break;
      case 8: /* first_prod ::= prod_name NEWLINE */
#line 57 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "first_prod ::= prod_name NEWLINE." << std::endl;
#endif
		}
		lsys->firstProductionName = sstringToString(*(yymsp[-1].minor.yy78.strValue));
		delete yymsp[-1].minor.yy78.strValue;
	}
}
#line 799 "conv_f8tof1_grammar.c"
        break;
      case 9: /* production ::= production_head SEMICOLON production_tail NEWLINE */
#line 69 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "production ::= production_head SEMICOLON production_tail NEWLINE." << std::endl;
#endif
		}
		Production *p = lsys->productions[sstringToString(*(yymsp[-3].minor.yy78.strValue))];
		for (vector<SString>::iterator paramIter = yymsp[-3].minor.yy78.vectorStr->begin(); paramIter != yymsp[-3].minor.yy78.vectorStr->end(); paramIter++) {
#if PARSER_DEBUG > 0
			std::cout << "1.1 " << *paramIter << std::endl;
			std::cout << *(yymsp[-3].minor.yy78.strValue) << " # " << (p == NULL) << " # " << true << std::endl;
#endif
			p->parameters.addParameter(*paramIter);
		}
		p->subproductions = *(yymsp[-1].minor.yy11.subproductions);
		delete yymsp[-3].minor.yy78.strValue;
		delete yymsp[-3].minor.yy78.vectorStr;
		delete yymsp[-1].minor.yy11.subproductions;
	}
}
#line 824 "conv_f8tof1_grammar.c"
        break;
      case 10: /* production_head ::= prod_name LPAREN prod_params RPAREN */
#line 91 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "production_head ::= prod_name LPAREN prod_params RPAREN." << std::endl;
#endif
		}
#if PARSER_DEBUG > 0
		std::cout << "---------" << *(yymsp[-3].minor.yy78.strValue) << std::endl;
#endif
		yygotominor.yy78.strValue = new SString(*(yymsp[-3].minor.yy78.strValue));
		yygotominor.yy78.vectorStr = new vector<SString>(*(yymsp[-1].minor.yy78.vectorStr));
		delete yymsp[-3].minor.yy78.strValue;
		delete yymsp[-1].minor.yy78.vectorStr;
	}
}
#line 844 "conv_f8tof1_grammar.c"
        break;
      case 11: /* production_head ::= prod_name LPAREN RPAREN */
#line 107 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "production_head ::= prod_name LPAREN RPAREN." << std::endl;
#endif
		}
#if PARSER_DEBUG > 0
		std::cout << "---------" << *(yymsp[-2].minor.yy78.strValue) << std::endl;
#endif
		yygotominor.yy78.strValue = new SString(*(yymsp[-2].minor.yy78.strValue));
		yygotominor.yy78.vectorStr = new vector<SString>();
		delete yymsp[-2].minor.yy78.strValue;
	}
}
#line 863 "conv_f8tof1_grammar.c"
        break;
      case 12: /* prod_params ::= param_name COMMA prod_params */
#line 123 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_params ::= param_name COMMA prod_params." << std::endl;
#endif
		}
		yygotominor.yy78.vectorStr = new vector<SString>();
		yygotominor.yy78.vectorStr->push_back(*(yymsp[-2].minor.yy78.strValue));
		for (vector<SString>::iterator iter = yymsp[0].minor.yy78.vectorStr->begin(); iter != yymsp[0].minor.yy78.vectorStr->end(); iter++) {
			yygotominor.yy78.vectorStr->push_back(*iter);
		}
		delete yymsp[-2].minor.yy78.strValue;
		delete yymsp[0].minor.yy78.vectorStr;
	}
}
#line 883 "conv_f8tof1_grammar.c"
        break;
      case 13: /* prod_params ::= param_name */
#line 139 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_params ::= param_name." << std::endl;
#endif
		}
		yygotominor.yy78.vectorStr = new vector<SString>();
		yygotominor.yy78.vectorStr->push_back(*(yymsp[0].minor.yy78.strValue));
		delete yymsp[0].minor.yy78.strValue;
	}
}
#line 899 "conv_f8tof1_grammar.c"
        break;
      case 14: /* production_tail ::= subproduction SEMICOLON production_tail */
#line 152 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "production_tail ::= subproduction SEMICOLON production_tail." << std::endl;
#endif
		}
		SubProduction sp;
		sp.conditions = *(yymsp[-2].minor.yy78.vectorConditions);
		vector<ActionStrP> actions;
		actions.reserve(yymsp[-2].minor.yy78.vectorActions->size());
		for (int i = 0; i < yymsp[-2].minor.yy78.vectorActions->size() && i < yymsp[-2].minor.yy78.parameters->size(); i++) {
			ActionStrP ap;
			ap.action = (*(yymsp[-2].minor.yy78.vectorActions)).at(i);
			ap.params = (*(yymsp[-2].minor.yy78.parameters)).at(i);
			actions.push_back(ap);
		}
		sp.actions = actions;
		yygotominor.yy11.subproductions = new vector<SubProduction>();
		yygotominor.yy11.subproductions->push_back(sp);
		for (vector<SubProduction>::iterator iter = yymsp[0].minor.yy11.subproductions->begin(); iter != yymsp[0].minor.yy11.subproductions->end(); iter++) {
			yygotominor.yy11.subproductions->push_back(*iter);
		}
		delete yymsp[-2].minor.yy78.vectorConditions;
		delete yymsp[-2].minor.yy78.vectorActions;
		delete yymsp[-2].minor.yy78.parameters;
		delete yymsp[0].minor.yy11.subproductions;
	}
}
#line 932 "conv_f8tof1_grammar.c"
        break;
      case 15: /* production_tail ::= subproduction */
#line 181 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "production_tail ::= subproduction." << std::endl;
#endif
		}
		SubProduction sp;
		sp.conditions = *(yymsp[0].minor.yy78.vectorConditions);
		vector<ActionStrP> actions;
		actions.reserve(yymsp[0].minor.yy78.vectorActions->size());
		for (int i = 0; i < yymsp[0].minor.yy78.vectorActions->size() && i < yymsp[0].minor.yy78.parameters->size(); i++) {
			ActionStrP ap;
			ap.action = (*(yymsp[0].minor.yy78.vectorActions)).at(i);
			ap.params = (*(yymsp[0].minor.yy78.parameters)).at(i);
			actions.push_back(ap);
		}
		sp.actions = actions;
		yygotominor.yy11.subproductions = new vector<SubProduction>();
		yygotominor.yy11.subproductions->push_back(sp);
		delete yymsp[0].minor.yy78.vectorConditions;
		delete yymsp[0].minor.yy78.vectorActions;
		delete yymsp[0].minor.yy78.parameters;
	}
}
#line 961 "conv_f8tof1_grammar.c"
        break;
      case 16: /* subproduction ::= conditions PIPE real_prod */
#line 207 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "subproduction ::= conditions PIPE real_prod." << std::endl;
#endif
		}
		yygotominor.yy78.vectorConditions = new vector<Condition>(*(yymsp[-2].minor.yy78.vectorConditions));
		yygotominor.yy78.vectorActions = new vector<Action*>(*(yymsp[0].minor.yy78.vectorActions));
		yygotominor.yy78.parameters = new vector<vector<SString> >(*(yymsp[0].minor.yy78.parameters));
		delete yymsp[-2].minor.yy78.vectorConditions;
		delete yymsp[0].minor.yy78.vectorActions;
		delete yymsp[0].minor.yy78.parameters;
	}
}
#line 980 "conv_f8tof1_grammar.c"
        break;
      case 17: /* subproduction ::= real_prod */
#line 222 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "subproduction ::= real_prod." << std::endl;
#endif
		}
		yygotominor.yy78.vectorConditions = new vector<Condition>();
		yygotominor.yy78.vectorActions = new vector<Action*>(*(yymsp[0].minor.yy78.vectorActions));
		yygotominor.yy78.parameters = new vector<vector<SString> >(*(yymsp[0].minor.yy78.parameters));
		delete yymsp[0].minor.yy78.vectorActions;
		delete yymsp[0].minor.yy78.parameters;
	}
}
#line 998 "conv_f8tof1_grammar.c"
        break;
      case 18: /* conditions ::= conditions COMMA conditions */
#line 237 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "conditions ::= condition COMMA conditions." << std::endl;
#endif
		}
		yygotominor.yy78.vectorConditions = new vector<Condition>(*(yymsp[-2].minor.yy78.vectorConditions));
		for (vector<Condition>::iterator iter = yymsp[0].minor.yy78.vectorConditions->begin(); 
			 iter != yymsp[0].minor.yy78.vectorConditions->end(); iter++) {
			yygotominor.yy78.vectorConditions->push_back(*iter);
		}	
		delete yymsp[-2].minor.yy78.vectorConditions;
		delete yymsp[0].minor.yy78.vectorConditions;
	}
}
#line 1018 "conv_f8tof1_grammar.c"
        break;
      case 19: /* conditions ::= condition */
#line 253 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "conditions ::= condition." << std::endl;
#endif
		}
		yygotominor.yy78.vectorConditions = new vector<Condition>();
		yygotominor.yy78.vectorConditions->push_back(*(yymsp[0].minor.yy78.cond));
		delete yymsp[0].minor.yy78.cond;
	}
}
#line 1034 "conv_f8tof1_grammar.c"
        break;
      case 20: /* condition ::= param_name LESS double_val */
#line 266 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "condition ::= param_name LESS double_val." << std::endl;
#endif
		}
		yygotominor.yy78.cond = new Condition();
		yygotominor.yy78.cond->relation = r_less;
		yygotominor.yy78.cond->parameter = *(yymsp[-2].minor.yy78.strValue);
		yygotominor.yy78.cond->value = yymsp[0].minor.yy78.dblValue;
		delete yymsp[-2].minor.yy78.strValue;
	}
}
#line 1052 "conv_f8tof1_grammar.c"
        break;
      case 21: /* condition ::= param_name LESS_EQUAL double_val */
#line 280 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "condition ::= param_name LESS_EQUAL double_val." << std::endl;
#endif
		}
		yygotominor.yy78.cond = new Condition();
		yygotominor.yy78.cond->relation = r_lessEqual;
		yygotominor.yy78.cond->parameter = *(yymsp[-2].minor.yy78.strValue);
		yygotominor.yy78.cond->value = yymsp[0].minor.yy78.dblValue;
		delete yymsp[-2].minor.yy78.strValue;
	}
}
#line 1070 "conv_f8tof1_grammar.c"
        break;
      case 22: /* condition ::= param_name NOT_EQUAL double_val */
#line 294 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "condition ::= param_name NOT_EQUAL double_val." << std::endl;
#endif
		}
		yygotominor.yy78.cond = new Condition();
		yygotominor.yy78.cond->relation = r_different;
		yygotominor.yy78.cond->parameter = *(yymsp[-2].minor.yy78.strValue);
		yygotominor.yy78.cond->value = yymsp[0].minor.yy78.dblValue;
		delete yymsp[-2].minor.yy78.strValue;
	}
}
#line 1088 "conv_f8tof1_grammar.c"
        break;
      case 23: /* condition ::= param_name EQUAL double_val */
#line 308 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "condition ::= param_name EQUAL double_val." << std::endl;
#endif
		}
		yygotominor.yy78.cond = new Condition();
		yygotominor.yy78.cond->relation = r_equal;
		yygotominor.yy78.cond->parameter = *(yymsp[-2].minor.yy78.strValue);
		yygotominor.yy78.cond->value = yymsp[0].minor.yy78.dblValue;
		delete yymsp[-2].minor.yy78.strValue;
	}
}
#line 1106 "conv_f8tof1_grammar.c"
        break;
      case 24: /* condition ::= param_name GREATER_EQUAL double_val */
#line 322 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "condition ::= param_name GREATER_EQUAL double_val." << std::endl;
#endif
		}
		yygotominor.yy78.cond = new Condition();
		yygotominor.yy78.cond->relation = r_greaterEqual;
		yygotominor.yy78.cond->parameter = *(yymsp[-2].minor.yy78.strValue);
		yygotominor.yy78.cond->value = yymsp[0].minor.yy78.dblValue;
		delete yymsp[-2].minor.yy78.strValue;
	}
}
#line 1124 "conv_f8tof1_grammar.c"
        break;
      case 25: /* condition ::= param_name GREATER double_val */
#line 336 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "condition ::= param_name GREATER double_val." << std::endl;
#endif
		}
		yygotominor.yy78.cond = new Condition();
		yygotominor.yy78.cond->relation = r_greater;
		yygotominor.yy78.cond->parameter = *(yymsp[-2].minor.yy78.strValue);
		yygotominor.yy78.cond->value = yymsp[0].minor.yy78.dblValue;
		delete yymsp[-2].minor.yy78.strValue;
	}
}
#line 1142 "conv_f8tof1_grammar.c"
        break;
      case 26: /* real_prod ::= */
#line 351 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "real_prod ::= ." << std::endl;
#endif
		}
		yygotominor.yy78.vectorActions = new vector<Action*>();
		yygotominor.yy78.parameters = new vector<vector<SString> >();
	}
}
#line 1157 "conv_f8tof1_grammar.c"
        break;
      case 27: /* real_prod ::= prod_piece real_prod */
#line 362 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "real_prod ::= prod_piece real_prod." << std::endl;
			for (vector<Action*>::iterator actIter = yymsp[-1].minor.yy89.actions->begin(); actIter != yymsp[-1].minor.yy89.actions->end(); actIter++) {
				cout << "\t" << (*actIter)->name << endl;
			}
#endif
		}
		//yygotominor.yy78 = yymsp[-1].minor.yy89.clone() && yygotominor.yy78.append(yymsp[0].minor.yy78)
		yygotominor.yy78.vectorActions = new vector<Action*>(*(yymsp[-1].minor.yy89.actions));
		yygotominor.yy78.parameters = new vector<vector<SString> >(*(yymsp[-1].minor.yy89.parameters));
		if (yymsp[0].minor.yy78.vectorActions != NULL && yymsp[0].minor.yy78.parameters != NULL) {
			for (vector<Action*>::iterator iter = yymsp[0].minor.yy78.vectorActions->begin(); iter != yymsp[0].minor.yy78.vectorActions->end(); iter++) {
				yygotominor.yy78.vectorActions->push_back(*iter);
			}
			for(vector<vector<SString> >::iterator iter = yymsp[0].minor.yy78.parameters->begin(); iter != yymsp[0].minor.yy78.parameters->end(); iter++) {
				yygotominor.yy78.parameters->push_back(*iter);
			}
			delete yymsp[0].minor.yy78.vectorActions;
			delete yymsp[0].minor.yy78.parameters;
		} else {
#if PARSER_DEBUG > 0
			cout << "\tNULL~~~~~~~~~~~~" << endl;
#endif
		}
		delete yymsp[-1].minor.yy89.actions;
		delete yymsp[-1].minor.yy89.parameters;
	}
}
#line 1192 "conv_f8tof1_grammar.c"
        break;
      case 28: /* real_prod ::= FORLOOP_BEGIN real_prod FORLOOP_END LPAREN single_val RPAREN real_prod */
#line 393 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "real_prod ::= FORLOOP_BEGIN real_prod FORLOOP_END LPAREN single_val RPAREN." << std::endl;
#endif
		}
		yygotominor.yy78.vectorActions = new vector<Action*>();
		yygotominor.yy78.parameters = new vector<vector<SString> >();
		int iterations = (int) parseExpression(*(yymsp[-2].minor.yy78.strValue));
		for (int i = 0; i < iterations; i++) {
			for (vector<Action*>::iterator iter = yymsp[-5].minor.yy78.vectorActions->begin(); iter != yymsp[-5].minor.yy78.vectorActions->end(); iter++) {
				yygotominor.yy78.vectorActions->push_back(*iter);
			}
			for (vector<vector<SString> >::iterator iter = yymsp[-5].minor.yy78.parameters->begin(); iter != yymsp[-5].minor.yy78.parameters->end(); iter++) {
				yygotominor.yy78.parameters->push_back(*iter);
			}
		}
		for (vector<Action*>::iterator iter = yymsp[0].minor.yy78.vectorActions->begin(); iter != yymsp[0].minor.yy78.vectorActions->end(); iter++) {
			yygotominor.yy78.vectorActions->push_back(*iter);
		}
		for (vector<vector<SString> >::iterator iter = yymsp[0].minor.yy78.parameters->begin(); iter != yymsp[0].minor.yy78.parameters->end(); iter++) {
			yygotominor.yy78.parameters->push_back(*iter);
		}
		delete yymsp[-5].minor.yy78.vectorActions;
		delete yymsp[-5].minor.yy78.parameters;
		delete yymsp[-2].minor.yy78.strValue;
		delete yymsp[0].minor.yy78.vectorActions;
		delete yymsp[0].minor.yy78.parameters;
	}
}
#line 1227 "conv_f8tof1_grammar.c"
        break;
      case 29: /* prod_piece ::= prod_name LPAREN multiple_val RPAREN */
#line 425 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_piece ::= prod_name LPAREN multiple_val RPAREN." << std::endl;
#endif
		}
		yygotominor.yy89.actions = new vector<Action*>();
		yygotominor.yy89.parameters = new vector<vector<SString> >();
		
		Production *p = lsys->productions.find(sstringToString(*(yymsp[-3].minor.yy78.strValue)))->second;
		yygotominor.yy89.actions->push_back(p);
		yygotominor.yy89.parameters->push_back(*(yymsp[-1].minor.yy78.vectorStr));
		delete yymsp[-3].minor.yy78.strValue;
		delete yymsp[-1].minor.yy78.vectorStr;
	}
}
#line 1248 "conv_f8tof1_grammar.c"
        break;
      case 30: /* prod_piece ::= prod_name LPAREN RPAREN */
#line 442 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_piece ::= prod_name LPAREN RPAREN." << std::endl;
#endif
		}
		yygotominor.yy89.actions = new vector<Action*>();
		vector<SString> param;
		yygotominor.yy89.parameters = new vector<vector<SString> >();
		yygotominor.yy89.parameters->push_back(param);
		
		Production *p = lsys->productions.find(sstringToString(*(yymsp[-2].minor.yy78.strValue)))->second;
		yygotominor.yy89.actions->push_back(p);
		delete yymsp[-2].minor.yy78.strValue;
	}
}
#line 1269 "conv_f8tof1_grammar.c"
        break;
      case 31: /* prod_piece ::= command */
#line 459 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_piece ::= command."  << std::endl;
#endif
		}
		yygotominor.yy89.actions = new vector<Action*>();
		yygotominor.yy89.parameters = new vector<vector<SString> >();
		PrimitiveProduction *pp = lsys->getPrimitiveProduction(*(yymsp[0].minor.yy78.strValue));
		yygotominor.yy89.actions->push_back(pp);
		vector<SString> param;
		yygotominor.yy89.parameters->push_back(param);
		delete yymsp[0].minor.yy78.strValue;
	}
}
#line 1289 "conv_f8tof1_grammar.c"
        break;
      case 32: /* prod_piece ::= param_name */
#line 475 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_piece ::= paramName." << std::endl;
#endif
		}
		yygotominor.yy89.actions = new vector<Action*>();
		yygotominor.yy89.parameters = new vector<vector<SString> >();
		ParamProduction *pp = lsys->getParamProduction(*(yymsp[0].minor.yy78.strValue));
		yygotominor.yy89.actions->push_back(pp);
		vector<SString> param;
		param.push_back(*(yymsp[0].minor.yy78.strValue));
		yygotominor.yy89.parameters->push_back(param);
		delete yymsp[0].minor.yy78.strValue;
	}
}
#line 1310 "conv_f8tof1_grammar.c"
        break;
      case 33: /* prod_piece ::= neuron */
#line 492 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_piece ::= neuron." << std::endl;
#endif
		}
		yygotominor.yy89.actions = new vector<Action*>();
		yygotominor.yy89.parameters = new vector<vector<SString> >();
		NeuronProduction *np = new NeuronProduction(*(yymsp[0].minor.yy78.strValue));
		lsys->neuronProductions.push_back(np);
		yygotominor.yy89.actions->push_back(np);
		vector<SString> param;
		yygotominor.yy89.parameters->push_back(param);
		delete yymsp[0].minor.yy78.strValue;		
	}
}
#line 1331 "conv_f8tof1_grammar.c"
        break;
      case 34: /* multiple_val ::= single_val */
#line 510 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "multiple_val ::= single_val." << std::endl;
#endif
		}
		yygotominor.yy78.vectorStr = new vector<SString>();
		yygotominor.yy78.vectorStr->push_back(SString(*(yymsp[0].minor.yy78.strValue)));
		delete yymsp[0].minor.yy78.strValue;
	}
}
#line 1347 "conv_f8tof1_grammar.c"
        break;
      case 35: /* multiple_val ::= multiple_val COMMA multiple_val */
#line 522 "conv_f8tof1_grammar.y"
{
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "multiple_val ::= multiple_val COMMA multiple_val." << std::endl;
#endif
		}
		yygotominor.yy78.vectorStr = new vector<SString>();
		for (vector<SString>::iterator iter = yymsp[-2].minor.yy78.vectorStr->begin(); iter != yymsp[-2].minor.yy78.vectorStr->end(); iter++) {
			yygotominor.yy78.vectorStr->push_back(*iter);
		}
		for (vector<SString>::iterator iter = yymsp[0].minor.yy78.vectorStr->begin(); iter != yymsp[0].minor.yy78.vectorStr->end(); iter++) {
			yygotominor.yy78.vectorStr->push_back(*iter);
		}
		delete yymsp[-2].minor.yy78.vectorStr;
		delete yymsp[0].minor.yy78.vectorStr;
	}
}
#line 1369 "conv_f8tof1_grammar.c"
        break;
      case 36: /* single_val ::= double_val */
#line 541 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "single_val ::= double_val." << std::endl;
#endif
		}
		yygotominor.yy78.strValue = new SString(SString::valueOf(yymsp[0].minor.yy78.dblValue) + ";");
		delete yymsp[0].minor.yy78.strValue;
	}
}
#line 1384 "conv_f8tof1_grammar.c"
        break;
      case 37: /* single_val ::= param_name */
#line 552 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "single_val ::= param_name." << std::endl;
#endif
		}
		yygotominor.yy78.strValue = new SString(*(yymsp[0].minor.yy78.strValue) + ";");
		delete yymsp[0].minor.yy78.strValue;
	}
}
#line 1399 "conv_f8tof1_grammar.c"
        break;
      case 38: /* single_val ::= single_val PLUS single_val */
#line 563 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "single_val ::= single_val PLUS single_val." << std::endl;
#endif
		}
		yygotominor.yy78.strValue = new SString(*(yymsp[-2].minor.yy78.strValue) + *(yymsp[0].minor.yy78.strValue) + "+;");
		delete yymsp[-2].minor.yy78.strValue;
		delete yymsp[0].minor.yy78.strValue;
	}
}
#line 1415 "conv_f8tof1_grammar.c"
        break;
      case 39: /* single_val ::= single_val MINUS single_val */
#line 575 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "single_val ::= single_val MINUS single_val." << std::endl;
#endif
		}
		yygotominor.yy78.strValue = new SString(*(yymsp[-2].minor.yy78.strValue) + *(yymsp[0].minor.yy78.strValue) + "-;");
		delete yymsp[-2].minor.yy78.strValue;
		delete yymsp[0].minor.yy78.strValue;
	}
}
#line 1431 "conv_f8tof1_grammar.c"
        break;
      case 40: /* single_val ::= single_val TIMES single_val */
#line 587 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "single_val ::= single_val TIMES single_val." << std::endl;
#endif
		}
		yygotominor.yy78.strValue = new SString(*(yymsp[-2].minor.yy78.strValue) + *(yymsp[0].minor.yy78.strValue) + "*;");
		delete yymsp[-2].minor.yy78.strValue;
		delete yymsp[0].minor.yy78.strValue;
	}
}
#line 1447 "conv_f8tof1_grammar.c"
        break;
      case 41: /* single_val ::= single_val DIV single_val */
#line 599 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "single_val ::= single_val DIV single_val." << std::endl;
#endif
		}
		yygotominor.yy78.strValue = new SString(*(yymsp[-2].minor.yy78.strValue) + *(yymsp[0].minor.yy78.strValue) + "/;");
		delete yymsp[-2].minor.yy78.strValue;
		delete yymsp[0].minor.yy78.strValue;
	}
}
#line 1463 "conv_f8tof1_grammar.c"
        break;
      case 42: /* double_val ::= DOUBLE_VAL */
#line 612 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "double_val ::= DOUBLE_VAL. -> " << (string(yymsp[0].minor.yy0.strArrValue)).c_str() << std::endl;
#endif
		}
		yygotominor.yy78.dblValue = atof((string(yymsp[0].minor.yy0.strArrValue)).c_str()); 
	}
}
#line 1477 "conv_f8tof1_grammar.c"
        break;
      case 43: /* param_name ::= PARAM_NAME */
#line 622 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "param_name ::= PARAM_NAME." << std::endl;
#endif
		}
		yygotominor.yy78.strValue = new SString(string(yymsp[0].minor.yy0.strArrValue).c_str());
	}
}
#line 1491 "conv_f8tof1_grammar.c"
        break;
      case 44: /* prod_name ::= PROD_NAME */
#line 632 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_name ::= PROD_NAME." << std::endl;
#endif
		}
		yygotominor.yy78.strValue = new SString(string(yymsp[0].minor.yy0.strArrValue).c_str());
	}
}
#line 1505 "conv_f8tof1_grammar.c"
        break;
      case 45: /* command ::= COMMAND */
#line 642 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "command ::= COMMAND." << std::endl;
#endif
		}
		yygotominor.yy78.strValue = new SString(string(yymsp[0].minor.yy0.strArrValue).c_str()); 
	}
}
#line 1519 "conv_f8tof1_grammar.c"
        break;
      case 46: /* neuron ::= NEURON */
#line 652 "conv_f8tof1_grammar.y"
{ 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "neuron ::= NEURON." << std::endl;
#endif
		}
		yygotominor.yy78.strValue = new SString(string(yymsp[0].minor.yy0.strArrValue).c_str()); 
	}
}
#line 1533 "conv_f8tof1_grammar.c"
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,yygoto);
  if( yyact < YYNSTATE ){
#ifdef NDEBUG
    /* If we are not debugging and the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = yyact;
      yymsp->major = yygoto;
      yymsp->minor = yygotominor;
    }else
#endif
    {
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else{
    assert( yyact == YYNSTATE + YYNRULE + 1 );
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  ParseARG_FETCH;
#define TOKEN (yyminor.yy0)
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ParseTOKENTYPE yyminor       /* The value for the token */
  ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      memset(&yyminorunion, 0, sizeof(yyminorunion));
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  ParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,yymajor);
    if( yyact<YYNSTATE ){
      assert( !yyendofinput );  /* Impossible to shift the $ token */
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      yymajor = YYNOCODE;
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}


