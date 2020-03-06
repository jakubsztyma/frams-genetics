// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <frams/genetics/fT/fTest_oper.h>

GenoOper_fTest gft;

//A demonstration of genetic operations on the "T" (fTest) genetic format:
int main()
{
	float chg;
	char *g = strdup(gft.getSimplest());
	for (int i = 0; i < 10; i++)
	{
		int _method; //unused
		/*int result = */gft.mutate(g, chg, _method);
		printf("%s   [mutated %.1f%%]\n", g, chg * 100);
	}

	char *g2 = strdup(gft.getSimplest());
	float chg2;
	printf("\nCrossing over the last mutant, \n\t%s\nand the simplest genotype\n\t%s\n:\n", g, g2);
	gft.crossOver(g, g2, chg, chg2);
	printf("Offspring 1:\n\t%s  (%.1f%% genes from parent1)\n", g, chg * 100);
	printf("Offspring 2:\n\t%s  (%.1f%% genes from parent2)\n", g2, chg2 * 100);
	free(g);
	free(g2);

	g = strdup("ATGsomethingCG");
	printf("\nChecking genotype:\n\t%s... error at position %d.\n", g, gft.checkValidity(g));
	gft.validate(g);
	printf("After validation:\n\t%s\n", g);
	free(g);
	printf("...and how about YOUR genotype?\n\n");

	return 0;
}
