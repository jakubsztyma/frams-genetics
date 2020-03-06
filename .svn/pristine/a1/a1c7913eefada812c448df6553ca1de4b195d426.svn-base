// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _F9_OPER_H_
#define _F9_OPER_H_

#include "../genooperators.h"


class GenoOper_f9 : public GenoOperators
{
public:
	GenoOper_f9();
	int checkValidity(const char *, const char *genoname);
	int validate(char *&, const char *genoname);
	int mutate(char *&g, float& chg, int &method);
	int crossOver(char *&g1, char *&g2, float& chg1, float& chg2);
	uint32_t style(const char *g, int pos);
	const char* getSimplest() { return "R"; }

	double mut_prob; //mutation probability
};

#endif
