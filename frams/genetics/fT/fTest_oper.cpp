// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "fTest_oper.h"
#include <common/nonstd.h> //randomN, rndDouble

/**
Sample output (simple examples of various genetic operations) produced by genooper_test_fTest.cpp:
$ ./genooper_test_fTest.exe
GATCGATTACA   [mutated 0.0%]
GATCCATTACA   [mutated 9.1%]
GATCCTGTACA   [mutated 27.3%]
GATCCTGTACA   [mutated 0.0%]
GATCCTGTACA   [mutated 0.0%]
GATCCTGTATA   [mutated 9.1%]
GATCCTGTATA   [mutated 0.0%]
GATACTGTATA   [mutated 9.1%]
GATACTGTATA   [mutated 9.1%]
GATACTGTATA   [mutated 0.0%]

Crossing over the last mutant,
GATACTGTATA
and the simplest genotype
GATCGATTACA
:
Offspring 1:
GATACTCGATTACA  (35.7% genes from parent1)
Offspring 2:
GATGTATA  (25.0% genes from parent2)

Checking genotype:
ATGsomethingCG... error at position 4.
After validation:
ATGCG
...and how about YOUR genotype?
*/


// To test this genetic format, you can also use the general genooper_test app that supports all genetic formats:
// $ ./genooper_test.exe /*T*/AAAAAAAAAAA



#define FIELDSTRUCT GenoOper_fTest
static ParamEntry GENOtestparam_tab[] =   //external access to ftest genetic parameters
{
	{ "Genetics: fTest", 1, 1, },
	{ "fTest_mut", 0, 0, "Mutation probability", "f 0 1", FIELD(prob), "How many genes should be mutated during single mutation (1=all genes, 0.1=ten percent)", },
	{ 0, },
};
#undef FIELDSTRUCT

GenoOper_fTest::GenoOper_fTest()
{
	par.setParamTab(GENOtestparam_tab);
	par.select(this);
	supported_format = 'T'; //'0' for f0, '1' for f1, 'F' for fF, etc.
	prob = 0.1;
}

///The only letters allowed are A,T,G,C
int GenoOper_fTest::checkValidity(const char* gene)
{
	if (!gene[0]) return 1; //empty is not valid
	bool ok = true;
	size_t i;
	for (i = 0; i < strlen(gene); i++) if (!strchr("ATGC", gene[i])) { ok = false; break; }
	return ok ? GENOPER_OK : i + 1;
}

///Remove all invalid letters from the genotype
int GenoOper_fTest::validate(char *&gene)
{
	SString validated; //new genotype (everything except ATGC is skipped)
	for (size_t i = 0; i < strlen(gene); i++)
		if (strchr("ATGC", gene[i])) validated += gene[i];  //validated contains only ATGC
	free(gene);
	gene = strdup(validated.c_str()); //reallocate
	return GENOPER_OK;
}

///Very simple mutation; should be improved to guarantee at least one gene changed
int GenoOper_fTest::mutate(char *&geno, float &chg, int &method)
{
	static char a[] = "ATGC";
	method = 0;
	int changes = 0, len = strlen(geno);
	for (int i = 0; i < len; i++)
		if (rndDouble(1) < prob) //normalize prob with length of genotype
		{
			geno[i] = a[rndUint(4)];
			changes++;
		}
	chg = (float)changes / len;
	return GENOPER_OK;
}

///A simple one-point crossover
int GenoOper_fTest::crossOver(char *&g1, char *&g2, float& chg1, float& chg2)
{
	int len1 = strlen(g1), len2 = strlen(g2);
	int p1 = rndUint(len1);  //random cut point for first genotype
	int p2 = rndUint(len2);  //random cut point for second genotype
	char *child1 = (char*)malloc(p1 + len2 - p2 + 1);
	char *child2 = (char*)malloc(p2 + len1 - p1 + 1);
	strncpy(child1, g1, p1);   strcpy(child1 + p1, g2 + p2);
	strncpy(child2, g2, p2);   strcpy(child2 + p2, g1 + p1);
	free(g1); g1 = child1;
	free(g2); g2 = child2;
	chg1 = (float)p1 / strlen(child1);
	chg2 = (float)p2 / strlen(child2);
	return GENOPER_OK;
}

///Applying some colors and font styles...
uint32_t GenoOper_fTest::style(const char *g, int pos)
{
	char ch = g[pos];
	uint32_t style = GENSTYLE_CS(0, GENSTYLE_INVALID); //default, should be changed below
	if (ch == 'A') style = GENSTYLE_RGBS(200, 0, 0, GENSTYLE_BOLD);
	if (ch == 'T') style = GENSTYLE_RGBS(0, 200, 0, GENSTYLE_BOLD);
	if (ch == 'G') style = GENSTYLE_RGBS(0, 0, 200, GENSTYLE_NONE);
	if (ch == 'C') style = GENSTYLE_RGBS(200, 200, 0, GENSTYLE_NONE);
	return style;
}
