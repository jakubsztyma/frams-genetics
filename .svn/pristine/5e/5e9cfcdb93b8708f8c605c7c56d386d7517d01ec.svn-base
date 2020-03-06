// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "fF_oper.h"
#include "fF_genotype.h"
#include <common/nonstd.h> //rndUint, rndDouble


#define FIELDSTRUCT GenoOper_fF
static ParamEntry GENOfFparam_tab[] =
{
	{ "Genetics: fF", 1, 1, },
	{ "fF_xover", 0, 0, "Inherited in linear mix crossover", "f 0.5 1.0 0.5", FIELD(xover_proportion), "0.5 => children are averaged parents.\n0.8 => children are only 20% different from parents.\n1.0 => each child is identical to one parent (no crossover).", },
	{ 0, },
};
#undef FIELDSTRUCT


GenoOper_fF::GenoOper_fF()
{
	par.setParamTab(GENOfFparam_tab);
	par.select(this);
	par.setDefault();
	supported_format = 'F';
}

int GenoOper_fF::checkValidity(const char* gene, const char *genoname)
{
	fF_growth_params par;
	return par.load(gene) ? GENOPER_OK : 1;
}

int GenoOper_fF::validate(char *&gene, const char *genoname)
{
	fF_growth_params par; //is initialized with default values
	par.load(gene); //loads as much as possible, other fields remain with default values
	string validated = par.save();
	free(gene);
	gene = strdup(validated.c_str()); //reallocate
	return GENOPER_OK;
}

//Creep-mutate one property
int GenoOper_fF::mutate(char *&gene, float &chg, int &method)
{
	method = 0;
	fF_growth_params par;
	par.load(gene);
	static const int propsToMutate[] = fF_PROPS_TO_MUTATE;
	int which = rndUint(ARRAY_LENGTH(propsToMutate));
	bool mutated_ok = GenoOperators::mutatePropertyNaive(par.param, propsToMutate[which]);
	if (mutated_ok)
	{
		string saved = par.save();
		free(gene);
		gene = strdup(saved.c_str()); //reallocate
		chg = 1.0f / par.param.getPropCount();
		return GENOPER_OK;
	}
	else
	{
		chg = 0;
		return GENOPER_OPFAIL;
	}
}

///Averaging crossover
int GenoOper_fF::crossOver(char *&g1, char *&g2, float& chg1, float& chg2)
{
	//g1 = strdup("1,0.5,0.5,0.5,0.5,1,1"); //testing...
	//g2 = strdup("4,1,  1,  1,  1,  2,2"); //testing...
	//xover_proportion = 0.1; //testing...
	fF_growth_params par1;
	par1.load(g1);
	fF_growth_params par2;
	par2.load(g2);
	chg1 = xover_proportion;
	chg2 = 1 - xover_proportion;
	for (int i = 0; i < par1.param.getPropCount(); i++)
		GenoOperators::linearMix(par1.param, i, par2.param, i, xover_proportion);
	string saved = par1.save();
	free(g1);
	g1 = strdup(saved.c_str()); //reallocate
	saved = par2.save();
	free(g2);
	g2 = strdup(saved.c_str()); //reallocate
	return GENOPER_OK;
}

///Applying some colors and font styles...
uint32_t GenoOper_fF::style(const char *g, int pos)
{
	char ch = g[pos];
	uint32_t style = GENSTYLE_CS(0, GENSTYLE_INVALID); //default, should be changed below
	if (strchr("-.e 0123456789", ch) != NULL)
		style = GENSTYLE_CS(GENCOLOR_NUMBER, GENSTYLE_NONE);
	else if (ch == ',')
		style = GENSTYLE_RGBS(0, 0, 0, GENSTYLE_BOLD);
	return style;
}
