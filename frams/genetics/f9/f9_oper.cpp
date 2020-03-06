// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "f9_oper.h"
#include "f9_conv.h"
#include <common/nonstd.h> //rndUint, rndDouble


#define FIELDSTRUCT GenoOper_f9
static ParamEntry GENOf9param_tab[] =
{
	{ "Genetics: f9", 1, 1, },
	{ "f9_mut", 0, 0, "Mutation probability", "f 0 1 0.1", FIELD(mut_prob), "How many genes should be mutated during single mutation (1=all genes, 0.1=ten percent)", },
	{ 0, },
};
#undef FIELDSTRUCT


GenoOper_f9::GenoOper_f9()
{
	par.setParamTab(GENOf9param_tab);
	par.select(this);
	par.setDefault();
	supported_format = '9';
}

int GenoOper_f9::checkValidity(const char* gene, const char *genoname)
{
	if (!gene[0]) return 1; //empty is not valid
	bool ok = true;
	size_t i;
	for (i = 0; i < strlen(gene); i++) if (!strchr(turtle_commands_f9, gene[i])) { ok = false; break; }
	return ok ? GENOPER_OK : i + 1;
}

///Remove all invalid letters from the genotype
int GenoOper_f9::validate(char *&gene, const char *genoname)
{
	SString validated; //new genotype (everything except turtle_commands_f9 is skipped)
	for (size_t i = 0; i < strlen(gene); i++)
		if (strchr(turtle_commands_f9, gene[i])) validated += gene[i];  //validated contains only turtle_commands_f9
	free(gene);
	gene = strdup(validated.c_str()); //reallocate
	return GENOPER_OK;
}

///Very simple mutation
int GenoOper_f9::mutate(char *&gene, float &chg, int &method)
{
	method = 0;
	int changes = 0, len = strlen(gene);
	int symbols = strlen(turtle_commands_f9);

	for (int i = 0; i < len; i++)
	{
		if (rndDouble(1) < mut_prob) //normalize prob with the length of the genotype
		{
			char oldgene = gene[i];
			gene[i] = turtle_commands_f9[rndUint(symbols)];
			if (gene[i] != oldgene) changes++;
		}
	}

	if (rndDouble(1) < mut_prob) //add or delete a random char
	{
		SString newgeno(gene);
		if (rndUint(2) == 0) //add
		{
			int symbols = strlen(turtle_commands_f9);
			int p = rndUint(len + 1);  //random location
			//printf("before add: %s\n",(const char*)newgeno);
			newgeno = newgeno.substr(0, p) + SString(turtle_commands_f9 + rndUint(symbols), 1) + newgeno.substr(p);
			//printf("after add: %s\n",(const char*)newgeno);
			changes++;
		}
		else if (len > 1) //delete
		{
			int p = rndUint(len);  //random location
			//printf("before delete: %s\n",(const char*)newgeno);
			newgeno = newgeno.substr(0, p) + newgeno.substr(p + 1);
			//printf("after delete: %s\n",(const char*)newgeno);
			changes++;
		}
		free(gene);
		gene = strdup(newgeno.c_str()); //reallocate
	}

	chg = (float)changes / len;
	return changes > 0 ? GENOPER_OK : GENOPER_OPFAIL; //no changes => OPFAIL so that GenMan will call mutate again
}

///A simple one-point crossover
int GenoOper_f9::crossOver(char *&g1, char *&g2, float& chg1, float& chg2)
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
uint32_t GenoOper_f9::style(const char *g, int pos)
{
	char ch = g[pos];
	uint32_t style = GENSTYLE_CS(0, GENSTYLE_INVALID); //default, should be changed below
	char *ptr = strchr((char*)turtle_commands_f9, ch);
	if (ptr)
	{
		int pos = ptr - turtle_commands_f9;
		int axis = pos / 2;
		style = GENSTYLE_RGBS(axis == 0 ? 200 : 0, axis == 1 ? 200 : 0, axis == 2 ? 200 : 0, GENSTYLE_NONE);
	}
	return style;
}
