// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "fn_oper.h"
#include "fn_conv.h"
#include <common/nonstd.h> //rndUint, rndDouble


/**
\class GenoOper_fn

This genetic representation only stores a vector of real numbers. A fitness function must be provided
for the gene pool, for example the "Booth function" would be:

var X = String.deserialize(this.geno.rawgenotype); //a vector of real values
var result = Math.pow(X[0]+2*X[1]-7,2) + Math.pow(2*X[0]+X[1]-5,2);
return -result; //negation because Framsticks assumes maximization, and the original function needs to be minimized
*/



#define FIELDSTRUCT GenoOper_fn
static ParamEntry GENOfnparam_tab[] =
{
	{ "Genetics: fn", 1, 6, },
	{ "fn_xover", 0, 0, "Fraction inherited in linear mix crossover", "f 0.5 1.0 0.9", FIELD(xover_proportion), "0.5 => children are averaged parents.\n0.8 => children are only 20% different from parents.\n1.0 => each child is identical to one parent (no crossover).", },
	{ "fn_xover_random", 0, 0, "Random fraction inherited in crossover", "d 0 1 1", FIELD(xover_proportion_random), "If active, the amount of linear mix is random in each crossover operation, so the \"Fraction inherited in linear mix crossover\" parameter is ignored.", },
	{ "fn_mut_bound_low", 1, 0, "Lower bounds for mutation", "s 0 0 [-10.0, -10.0]", FIELD(mut_bound_low), "A vector of lower bounds (one real value for each variable)", },
	{ "fn_mut_bound_high", 1, 0, "Higher bounds for mutation", "s 0 0 [10.0, 10.0]", FIELD(mut_bound_high), "A vector of higher bounds (one real value for each variable)", },
	{ "fn_mut_stddev", 1, 0, "Standard deviations for mutation", "s 0 0 [0.1, 0.1]", FIELD(mut_stddev), "A vector of standard deviations (one real value for each variable)", },
	{ "fn_mut_single_var", 0, 0, "Mutate only a single variable", "d 0 1 0", FIELD(mut_single_var), "If active, only a single randomly selected variable will be mutated in each mutation operation. Otherwise all variables will be mutated.", },
	{ 0, },
};
#undef FIELDSTRUCT



GenoOper_fn::GenoOper_fn()
{
	par.setParamTab(GENOfnparam_tab);
	par.select(this);
	par.setDefault();
	supported_format = 'n';
}

int GenoOper_fn::checkValidity(const char* gene, const char *genoname)
{
	vector<double> values = GenoConv_fn0::stringToVector(gene);
	return values.size() > 0 ? GENOPER_OK : 1;
}

int GenoOper_fn::validate(char *&gene, const char *genoname)
{
	vector<double> values = GenoConv_fn0::stringToVector(gene);
	if (values.size() == 0)
		values.push_back(0.0);
	string validated = GenoConv_fn0::vectorToString(values);
	free(gene);
	gene = strdup(validated.c_str()); //reallocate
	return GENOPER_OK;
}

//Creep-mutate variable(s)
int GenoOper_fn::mutate(char *&gene, float &chg, int &method)
{
	method = 0;
	vector<double> values = GenoConv_fn0::stringToVector(gene);
	if (values.size() == 0)
		return GENOPER_OPFAIL;
	vector<double> bound_low = GenoConv_fn0::stringToVector(mut_bound_low.c_str());
	vector<double> bound_high = GenoConv_fn0::stringToVector(mut_bound_high.c_str());
	vector<double> stddev = GenoConv_fn0::stringToVector(mut_stddev.c_str());
	if (bound_low.size() != bound_high.size() || bound_high.size() != stddev.size() || stddev.size() != values.size())
	{
		logPrintf("GenoOper_fn", "mutate", LOG_ERROR, "The solution vector, bound vectors, and standard deviation vectors must all have the same number of values");
		return GENOPER_OPFAIL;
	}

	if (mut_single_var) //mutate only one, randomly selected variable
	{
		int which = rndUint(values.size());
		values[which] = GenoOperators::mutateCreep('f', values[which], bound_low[which], bound_high[which], stddev[which], false);
		chg = 1.0f / values.size();
	}
	else //mutate all variables
	{
		for (int which = 0; which < (int)values.size(); which++)
			values[which] = GenoOperators::mutateCreep('f', values[which], bound_low[which], bound_high[which], stddev[which], false);
		chg = 1.0f;
	}
	string saved = GenoConv_fn0::vectorToString(values);
	free(gene);
	gene = strdup(saved.c_str()); //reallocate
	return GENOPER_OK;
}

//Averaging crossover
int GenoOper_fn::crossOver(char *&g1, char *&g2, float& chg1, float& chg2)
{
	//g1 = strdup("[1,0.5,0.5,0.5,0.5,1,1]"); //testing...
	//g2 = strdup("[4,1,  1,  1,  1,  2,2]"); //testing...
	//xover_proportion = 0.1; //testing...

	double proportion = xover_proportion_random ? 0.5 + rndDouble(0.5) : xover_proportion;

	chg1 = proportion;
	chg2 = 1 - proportion;

	vector<double> v1 = GenoConv_fn0::stringToVector(g1);
	vector<double> v2 = GenoConv_fn0::stringToVector(g2);

	if (v1.size() != v2.size())
	{
		logPrintf("GenoOper_fn", "crossOver", LOG_ERROR, "Tried to cross over solutions with a differing number of variables (%d and %d)", v1.size(), v2.size());
		return GENOPER_OPFAIL;
	}

	GenoOperators::linearMix(v1, v2, proportion);

	string saved = GenoConv_fn0::vectorToString(v1);
	free(g1);
	g1 = strdup(saved.c_str()); //reallocate
	saved = GenoConv_fn0::vectorToString(v2);
	free(g2);
	g2 = strdup(saved.c_str()); //reallocate
	return GENOPER_OK;
}

//Applying some colors and font styles...
uint32_t GenoOper_fn::style(const char *g, int pos)
{
	char ch = g[pos];
	uint32_t style = GENSTYLE_CS(0, GENSTYLE_INVALID); //default, should be changed below
	if (strchr("-.e 0123456789", ch) != NULL)
		style = GENSTYLE_CS(GENCOLOR_NUMBER, GENSTYLE_NONE);
	else if (strchr("[,]", ch) != NULL)
		style = GENSTYLE_RGBS(0, 0, 0, GENSTYLE_BOLD);
	return style;
}
