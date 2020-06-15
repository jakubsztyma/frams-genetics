// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 2019-2020  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <float.h>
#include "fS_oper.h"


#define FIELDSTRUCT GenoOper_fS
static ParamEntry GENOfSparam_tab[] =
		{
				{"Genetics: fS",            1, FS_OPCOUNT + 4,},
				{"fS_mut_add_part",         0, 0, "Add part",                 "f 0 100 10", FIELD(prob[FS_ADD_PART]),             "mutation: probability of adding a part",},
				{"fS_mut_rem_part",         0, 0, "Remove part",              "f 0 100 10", FIELD(prob[FS_REM_PART]),             "mutation: probability of deleting a part",},
				{"fS_mut_mod_part",         0, 0, "Modify part",              "f 0 100 10", FIELD(prob[FS_MOD_PART]),             "mutation: probability of changing the part type",},
				{"fS_mut_add_joint",        0, 0, "Add joint",                "f 0 100 10", FIELD(prob[FS_ADD_JOINT]),            "mutation: probability of adding a joint",},
				{"fS_mut_rem_joint",        0, 0, "Remove joint",             "f 0 100 10", FIELD(prob[FS_REM_JOINT]),            "mutation: probability of removing a joint",},
				{"fS_mut_add_param",        0, 0, "Add param",                "f 0 100 10", FIELD(prob[FS_ADD_PARAM]),            "mutation: probability of adding a parameter",},
				{"fS_mut_rem_param",        0, 0, "Remove param",             "f 0 100 10", FIELD(prob[FS_REM_PARAM]),            "mutation: probability of removing a parameter",},
				{"fS_mut_mod_param",        0, 0, "Modify param",             "f 0 100 10", FIELD(prob[FS_MOD_PARAM]),            "mutation: probability of modifying a parameter",},
				{"fS_mut_add_mod",          0, 0, "Add modifier",             "f 0 100 10", FIELD(prob[FS_ADD_MOD]),              "mutation: probability of adding a modifier",},
				{"fS_mut_rem_mod",          0, 0, "Remove modifier",          "f 0 100 10", FIELD(prob[FS_REM_MOD]),              "mutation: probability of deleting a modifier",},
				{"fS_mut_add_neuro",        0, 0, "Add neuron",               "f 0 100 10", FIELD(prob[FS_ADD_NEURO]),            "mutation: probability of adding a neuron",},
				{"fS_mut_rem_neuro",        0, 0, "Remove neuron",            "f 0 100 10", FIELD(prob[FS_REM_NEURO]),            "mutation: probability of removing a neuron",},
				{"fS_mut_mod_neuro",        0, 0, "Modify neuron",            "f 0 100 10", FIELD(prob[FS_MOD_NEURO_CONNECTION]), "mutation: probability of changing a neuron connection",},
				{"fS_mut_add_neuro_conn",   0, 0, "Add neuron connection",    "f 0 100 10", FIELD(prob[FS_ADD_NEURO_CONNECTION]), "mutation: probability of adding a neuron connection",},
				{"fS_mut_rem neuro_conn",   0, 0, "Remove neuron connection", "f 0 100 10", FIELD(prob[FS_REM_NEURO_CONNECTION]), "mutation: probability of removing a neuron connection",},
				{"fS_mut_mod_neuro_params", 0, 0, "Modify neuron params",     "f 0 100 10", FIELD(prob[FS_MOD_NEURO_PARAMS]),     "mutation: probability of changing a neuron param",},
				{"fS_circle_section",       0, 0, "Ensure circle section",    "d 0 1 1",    FIELD(ensureCircleSection),           "Ensure that ellipsoids and cylinders have circle cross-section"},
				{"fS_use_elli",       0, 0, "Use ellipsoids in mutations",    "d 0 1 1",    FIELD(useElli),           "Use ellipsoids in mutations"},
				{"fS_use_cub",       0, 0, "Use cuboids in mutations",    "d 0 1 1",    FIELD(useCub),           "Use cuboids in mutations"},
				{"fS_use_cyl",       0, 0, "Use cylinders in mutations",    "d 0 1 1",    FIELD(useCyl),           "Use cylinders in mutations"},
		};

#undef FIELDSTRUCT

GenoOper_fS::GenoOper_fS()
{
	par.setParamTab(GENOfSparam_tab);
	par.select(this);
	par.setDefault();
	supported_format = 'S';
}

int GenoOper_fS::checkValidity(const char *geno, const char *genoname)
{
	try
	{
		fS_Genotype genotype = fS_Genotype(geno);
		if(!genotype.allPartSizesValid())
		{
			logPrintf("GenoOper_fS", "checkValidity", LOG_ERROR, "Wrong part size");
			return 1;
		}
	}
	catch (fS_Exception &e)
	{
		logPrintf("GenoOper_fS", "checkValidity", LOG_ERROR, e.what());
		return e.errorPosition;
	}
	return 0;
}


int GenoOper_fS::mutate(char *&geno, float &chg, int &method)
{
	fS_Genotype genotype(geno);

	// Calculate available part types
	string availableTypes;
	if(useElli)
		availableTypes += ELLIPSOID;
	if(useCub)
		availableTypes += CUBOID;
	if(useCyl)
		availableTypes += CYLINDER;

	// Select a mutation
	bool result = false;
	method = GenoOperators::roulette(prob, FS_OPCOUNT);
	switch (method)
	{
		case FS_ADD_PART:
			result = genotype.addPart(ensureCircleSection, availableTypes);
			break;
		case FS_REM_PART:
			result = genotype.removePart();
			break;
		case FS_MOD_PART:
			result = genotype.changePartType(ensureCircleSection, availableTypes);
			break;
		case FS_ADD_JOINT:
			result = genotype.addJoint();
			break;
		case FS_REM_JOINT:
			result = genotype.removeJoint();
			break;
		case FS_ADD_PARAM:
			result = genotype.addParam(ensureCircleSection);
			break;
		case FS_REM_PARAM:
			result = genotype.removeParam();
			break;
		case FS_MOD_PARAM:
			result = genotype.changeParam(ensureCircleSection);
			break;
		case FS_ADD_MOD:
			result = genotype.addModifier();
			break;
		case FS_REM_MOD:
			result = genotype.removeModifier();
			break;
		case FS_ADD_NEURO:
			result = genotype.addNeuro();
			break;
		case FS_REM_NEURO:
			result = genotype.removeNeuro();
			break;
		case FS_MOD_NEURO_CONNECTION:
			result = genotype.changeNeuroConnection();
			break;
		case FS_ADD_NEURO_CONNECTION:
			result = genotype.addNeuroConnection();
			break;
		case FS_REM_NEURO_CONNECTION:
			result = genotype.removeNeuroConnection();
			break;
		case FS_MOD_NEURO_PARAMS:
//			result = genotype.changeNeuroParam();
			break;
	}

	if (result)
	{
		free(geno);
		geno = strdup(genotype.getGeno().c_str());
		return GENOPER_OK;
	}
	return GENOPER_OPFAIL;
}

int GenoOper_fS::crossOver(char *&g0, char *&g1, float &chg0, float &chg1)
{
	fS_Genotype *parents[PARENT_COUNT] = {new fS_Genotype(g0), new fS_Genotype(g1)};

	// Choose random subtrees that have similar size
	Node *selected[PARENT_COUNT];
	vector<Node*> allNodes0 = parents[0]->getAllNodes();
	vector<Node*> allNodes1 = parents[1]->getAllNodes();

	double bestQuotient = DBL_MAX;
	for (int i = 0; i < crossOverTries; i++)
	{
		Node *tmp0 = allNodes0[rndUint(allNodes0.size())];
		Node *tmp1 = allNodes1[rndUint(allNodes1.size())];
		// Choose this pair if it is the most similar
		double quotient = double(tmp0->getNodeCount()) / double(tmp1->getNodeCount());
		if(quotient < 1.0)
			quotient = 1.0 / quotient;
		if (quotient < bestQuotient)
		{
			bestQuotient = quotient;
			selected[0] = tmp0;
			selected[1] = tmp1;
		}
		if (bestQuotient == 1.0)
			break;
	}

	// Compute gene percentages in children
	double subtreeSizes[PARENT_COUNT], restSizes[PARENT_COUNT];
	for (int i = 0; i < PARENT_COUNT; i++)
	{

		subtreeSizes[i] = selected[i]->getNodeCount();
		restSizes[i] = parents[i]->getNodeCount() - subtreeSizes[i];
	}
	chg0 = restSizes[0] / (restSizes[0] + subtreeSizes[1]);
	chg1 = restSizes[1] / (restSizes[1] + subtreeSizes[0]);

	// Rearrange neurons before crossover
	int subOldStart[PARENT_COUNT] {-1, -1};
	rearrangeConnectionsBeforeCrossover(parents[0], selected[0], subOldStart[0]);
	rearrangeConnectionsBeforeCrossover(parents[1], selected[1], subOldStart[1]);

	// Swap the subtress
	for(int i=0; i<PARENT_COUNT; i++)
	{
		Node *other = selected[1 - i];
		Node *p = selected[i]->parent;
		if (p != nullptr)
		{
			size_t index = std::distance(p->children.begin(), std::find(p->children.begin(), p->children.end(), selected[i]));
			p->children[index] = other;
		} else
			parents[i]->startNode = other;
	}

	// Rearrange neurons after crossover
	rearrangeConnectionsAfterCrossover(parents[0], selected[1], subOldStart[0]);
	rearrangeConnectionsAfterCrossover(parents[1], selected[0], subOldStart[1]);

	// Clenup, assign children to result strings
	free(g0);
	free(g1);
	g0 = strdup(parents[0]->getGeno().c_str());
	g1 = strdup(parents[1]->getGeno().c_str());

	delete parents[0];
	delete parents[1];
	return GENOPER_OK;
}

const char* GenoOper_fS::getSimplest()
{
	return "S:C{x=0.80599;y=0.80599;z=0.80599}";
}

uint32_t GenoOper_fS::style(const char *geno, int pos)
{
	char ch = geno[pos];
	uint32_t style = GENSTYLE_CS(0, GENSTYLE_NONE);
	if (ch == ELLIPSOID || ch == CUBOID || ch == CYLINDER) // part type
	{
		style = GENSTYLE_RGBS(0, 0, 200, GENSTYLE_BOLD);
	}
	else if(JOINTS.find(ch) != string::npos)	// Joint type
	{
		style = GENSTYLE_RGBS(0, 200, 200, GENSTYLE_BOLD);
	}
	else if(MODIFIERS.find(ch) != string::npos) // Modifier
	{
		style = GENSTYLE_RGBS(0, 200, 0, GENSTYLE_NONE);
	}
	else if (isdigit(ch) || strchr(".=", ch)) // Numerical value
	{
		style = GENSTYLE_RGBS(200, 0, 0, GENSTYLE_NONE);
	}
	else if(strchr("()_;[],", ch))
	{
		style = GENSTYLE_CS(0, GENSTYLE_BOLD); // Important char
	}

	return style;
}

void GenoOper_fS::rearrangeConnectionsBeforeCrossover(fS_Genotype *geno, Node *sub, int &subStart)
{
	vector<fS_Neuron*> genoNeurons = geno->getAllNeurons();
	vector<fS_Neuron*> subNeurons = fS_Genotype::extractNeurons(sub);

	if (!subNeurons.empty())
	{
		subStart = fS_Genotype::getNeuronIndex(genoNeurons, subNeurons[0]);
		fS_Genotype::shiftNeuroConnections(genoNeurons, subStart, subStart + subNeurons.size() - 1, SHIFT::LEFT);
	}
}

void GenoOper_fS::rearrangeConnectionsAfterCrossover(fS_Genotype *geno, Node *sub, int subOldStart)
{
	vector<fS_Neuron*> genoNeurons = geno->getAllNeurons();
	vector<fS_Neuron*> subNeurons = fS_Genotype::extractNeurons(sub);

	// Shift the inputs right
	if (!subNeurons.empty())
	{
		int subStart = fS_Genotype::getNeuronIndex(genoNeurons, subNeurons[0]);
		int subCount = subNeurons.size();
		int subEnd = subStart + subCount - 1;
		for (int i = 0; i < subCount; i++)
		{
			auto inputs = subNeurons[i]->inputs;
			std::map<int, double> newInputs;
			// TODO figure out how to keep internal connections in subtree
//			for (auto it = inputs.begin(); it != inputs.end(); ++it)
//			{
//				int newIndex = it->first + subStart;
//				if(subEnd > newIndex && newIndex > subStart)
//					newInputs[newIndex] = it->second;
//			}
			subNeurons[i]->inputs = newInputs;
		}
		fS_Genotype::shiftNeuroConnections(genoNeurons, subStart, subEnd, SHIFT::RIGHT);
	}
}

