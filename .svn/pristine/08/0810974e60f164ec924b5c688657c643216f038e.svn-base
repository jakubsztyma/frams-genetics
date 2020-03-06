// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <common/loggers/loggertostdout.h>
#include <frams/genetics/preconfigured.h>

void printGen(Geno &g)
{
	printf("Genotype: %s\nFormat: %c\nValid: %s\nComment: %s\n",
		g.getGenes().c_str(), g.getFormat(), g.isValid() ? "yes" : "no", g.getComment().len() == 0 ? "(empty)" : g.getComment().c_str());
}

void printGenAndTitle(Geno &g, const char* title)
{
	printf("\n--------------------- %s: ---------------------\n", title);
	printGen(g);
}

/* Demonstrates various genetic operators applied to a sample genotype. See also oper_fx.cpp. */
int main(int argc, char *argv[])
{
	LoggerToStdout messages_to_stdout(LoggerBase::Enable);
	PreconfiguredGenetics genetics;

	rndGetInstance().randomize();
	genetics.genman.p_report(NULL, NULL);

	const char* src = (argc > 1) ? argv[1] : "/*9*/UUU";
	Geno gsrc(src, -1, "First");
	printGenAndTitle(gsrc, "source genotype (gsrc)");
	char format = gsrc.getFormat();

	Geno gmut = genetics.genman.mutate(gsrc);
	printGenAndTitle(gmut, "mutated (gmut)");

	Geno gxover = genetics.genman.crossOver(gsrc, gmut);
	printGenAndTitle(gxover, "crossed over (gsrc and gmut)");

	Geno gsimplest = genetics.genman.getSimplest(format);
	printGenAndTitle(gsimplest, "simplest");

	Geno ginvalid("IT'S REALLY WRONG", format);
	printGenAndTitle(ginvalid, "invalid");

	Geno gvalidated = genetics.genman.validate(ginvalid);
	printGenAndTitle(gvalidated, "validated");

	printf("\nHTMLized: %s\n", genetics.genman.HTMLize(gvalidated.getGenes().c_str()).c_str());

	return 0;
}
