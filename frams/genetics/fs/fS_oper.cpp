//
// Created by jakub on 19.03.2020.
//

#include "fS_oper.h"


#define FIELDSTRUCT GenoOper_fS
static ParamEntry GENOfSparam_tab[] =
		{
				{"Genetics: fS",            1, FS_OPCOUNT + 1,},
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
		};

#undef FIELDSTRUCT

GenoOper_fS::GenoOper_fS()
{
	par.setParamTab(GENOfSparam_tab);
	par.select(this);
	par.setDefault();
}

int GenoOper_fS::checkValidity(const char *geno, const char *genoname)
{
	try
	{
		fS_Genotype genotype = fS_Genotype(geno);
		if (!genotype.allPartSizesValid())
			return 1;
	}
	catch (fS_Exception &e)
	{
		logPrintf("GenoOper_fS", "checkValidity", LOG_ERROR, e.what());
		return 1;
	}
	return 0;
}


int GenoOper_fS::mutate(char *&geno, float &chg, int &method)
{
	fS_Genotype genotype(geno);

	bool result = false;
	method = GenoOperators::roulette(prob, FS_OPCOUNT);
	switch (method)
	{
		case FS_ADD_PART:
			result = genotype.addPart(ensureCircleSection);
			break;
		case FS_REM_PART:
			result = genotype.removePart();
			break;
		case FS_MOD_PART:
			result = genotype.changePartType(ensureCircleSection);
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
			result = genotype.changeNeuroParam();
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
	int parentCount = 2;
	fS_Genotype *parents[parentCount] = {new fS_Genotype(g0), new fS_Genotype(g1)};

	if (parents[0]->startNode->children.empty() || parents[1]->startNode->children.empty())
	{
		delete parents[0];
		delete parents[1];
		return GENOPER_OPFAIL;
	}

	Node *selected[parentCount];
	int childIndexes[parentCount];
	// Choose random subtrees that have similar size
	vector<Node*> allNodes[parentCount]
	{
		parents[0]->getAllNodes(),
				parents[1]->getAllNodes()
	};
	double bestQuotient = DBL_MAX;
	for (int i = 0; i < crossOverTries; i++)
	{
		Node *selectedTmp[parentCount];
		int childIndexesTmp[parentCount];
		double childNodeCount[parentCount];
		for (int i = 0; i < parentCount; i++)
		{
			do
			{
				selectedTmp[i] = allNodes[i][rndUint(allNodes[i].size())];
			} while (selectedTmp[i]->children.empty());
			childIndexesTmp[i] = rndUint(selectedTmp[i]->children.size());
			childNodeCount[i] = selectedTmp[i]->children[childIndexesTmp[i]]->getNodeCount();
		}
		// Choose the most similar subtrees
		double quotient = std::max(childNodeCount[0], childNodeCount[1]) / std::min(childNodeCount[0], childNodeCount[1]);
		if (quotient < bestQuotient)
		{
			bestQuotient = quotient;
			for (int i = 0; i < parentCount; i++)
			{
				selected[i] = selectedTmp[i];
				childIndexes[i] = childIndexesTmp[i];
			}
		}
		if (quotient == 1.0)
			break;
	}

	// Compute gene percentages in children
	double subtreeSizes[parentCount], restSizes[parentCount];
	for (int i = 0; i < parentCount; i++)
	{

		subtreeSizes[i] = selected[i]->children[childIndexes[i]]->getNodeCount();
		restSizes[i] = parents[i]->getNodeCount() - subtreeSizes[i];
	}
	chg0 = restSizes[0] / (restSizes[0] + subtreeSizes[1]);
	chg1 = restSizes[1] / (restSizes[1] + subtreeSizes[0]);

	// Rearrange neurons before crossover
	Node *subtrees[2];
	subtrees[0] = selected[0]->children[childIndexes[0]];
	subtrees[1] = selected[1]->children[childIndexes[1]];

	int subOldStart[parentCount] {-1, -1};
	rearrangeConnectionsBeforeCrossover(parents[0], subtrees[0], subOldStart[0]);
	rearrangeConnectionsBeforeCrossover(parents[1], subtrees[1], subOldStart[0]);

	// Swap the subtress
	std::swap(selected[0]->children[childIndexes[0]], selected[1]->children[childIndexes[1]]);

	// Rearrange neurons after crossover
	rearrangeConnectionsAfterCrossover(parents[0], subtrees[1], subOldStart[0]);
	rearrangeConnectionsAfterCrossover(parents[1], subtrees[0], subOldStart[1]);

	// Clenup, assign children to result strings
	free(g0);
	free(g1);
	g0 = strdup(parents[0]->getGeno().c_str());
	g1 = strdup(parents[1]->getGeno().c_str());

	delete parents[0];
	delete parents[1];
	return GENOPER_OK;
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