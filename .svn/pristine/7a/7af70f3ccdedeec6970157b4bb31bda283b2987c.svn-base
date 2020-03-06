// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FTEST_OPER_H_
#define _FTEST_OPER_H_

#include "../genooperators.h"

/** \file */

///A sample class for genetic operations on DNA-like genotypes
/**
\author Maciej Komosinski

This is a very simple class that illustrates basic genetic operations performed on ATGC sequences.
For a more sophisticated and realistic examples of genetic formats and operators derived from GenoOperators,
refer to the available source for genetic formats f9, fF, and f4.

\sa \ref geno_ftest_example
*/

class GenoOper_fTest : public GenoOperators
{
public:
	GenoOper_fTest();
	int checkValidity(const char *);
	int validate(char *&);
	int mutate(char *&geno, float& chg, int &method);
	int crossOver(char *&g1, char *&g2, float& chg1, float& chg2);
	uint32_t style(const char *g, int pos);
	const char* getSimplest() { return "GATCGATTACA"; }

	double prob;
};

#endif
