/*
 *  lexglobal.h
 *  L-systemToF1
 *
 *  Created by Maciej Wajcht on 08-03-28.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef YYSTYPE
#include "sstring.h"

typedef struct {
	double dblVal;
	SString strVal;
	struct symtab *symp;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1

#endif

/* extern YYSTYPE yylval; */
//YYSTYPE yylval;
