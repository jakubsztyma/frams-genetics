// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2017  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FF_OPER_H_
#define _FF_OPER_H_

#include "../genooperators.h"


class GenoOper_fF : public GenoOperators
{
public:
	GenoOper_fF();
	int checkValidity(const char *, const char *genoname);
	int validate(char *&, const char *genoname);
	int mutate(char *&g, float& chg, int &method);
	int crossOver(char *&g1, char *&g2, float& chg1, float& chg2);
	uint32_t style(const char *g, int pos);
	const char* getSimplest() { return "6, 0.1, 0.1, 0.1, 1.05, 1.05, 1.05, 0, 0, 0"; }

	double xover_proportion;
};

#endif

