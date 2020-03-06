// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _Fn_OPER_H_
#define _Fn_OPER_H_

#include "../genooperators.h"


class GenoOper_fn : public GenoOperators
{
public:
	GenoOper_fn();
	int checkValidity(const char *, const char *genoname);
	int validate(char *&, const char *genoname);
	int mutate(char *&g, float& chg, int &method);
	int crossOver(char *&g1, char *&g2, float& chg1, float& chg2);
	uint32_t style(const char *g, int pos);
	const char* getSimplest() { return "[0.0, 0.0]"; } //should actually correspond in length to vectors in mut_bound_low, mut_bound_high, mut_stddev

	double xover_proportion;
	int xover_proportion_random, mut_single_var;
	SString mut_bound_low, mut_bound_high, mut_stddev;
};

#endif
