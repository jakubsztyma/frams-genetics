//
// Created by jakub on 19.03.2020.
//

#include "fS_oper.h"


#define FIELDSTRUCT fS_Operators
static ParamEntry GENOfSparam_tab[] =
		{
				{"Genetics: fS",            1, FS_OPCOUNT,},
				{"fS_mut_add",              0, 0, "Add part",                 "f 0 100 10", FIELD(prob[FS_ADD_PART]),                   "mutation: probability of adding a part",},
				{"fS_mut_rem",               0, 0, "Remove part",              "f 0 100 10", FIELD(prob[FS_REM_PART]),                      "mutation: probability of deleting a part",},
				{"fS_mut_mod",              0, 0, "Modify part",              "f 0 100 10", FIELD(prob[FS_MOD_PART]),                  "mutation: probability of changing the part type",},
				{"fS_mut_add_joint",        0, 0, "Add joint",                "f 0 100 10", FIELD(prob[FS_ADD_JOINT]),            "mutation: probability of adding a joint",},
				{"fS_mut_rem_joint",         0, 0, "Remove joint",             "f 0 100 10", FIELD(prob[FS_REM_JOINT]),              "mutation: probability of removing a joint",},
				{"fS_mut_add_param",        0, 0, "Add param",                "f 0 100 10", FIELD(prob[FS_ADD_PARAM]),            "mutation: probability of adding a parameter",},
				{"fS_mut_rem_param",         0, 0, "Remove param",             "f 0 100 10", FIELD(prob[FS_REM_PARAM]),             "mutation: probability of removing a parameter",},
				{"fS_mut_mod_param",        0, 0, "Modify param",             "f 0 100 10", FIELD(prob[FS_MOD_PARAM]),             "mutation: probability of modifying a parameter",},
				{"fS_mut_add_mod",          0, 0, "Add modifier",             "f 0 100 10", FIELD(prob[FS_ADD_MOD]),              "mutation: probability of adding a modifier",},
				{"fS_mut_rem_mod",           0, 0, "Remove modifier",          "f 0 100 10", FIELD(prob[FS_REM_MOD]),                "mutation: probability of deleting a modifier",},
				{"fS_mut_add_neuro",        0, 0, "Add neuron",               "f 0 100 10", FIELD(prob[FS_ADD_NEURO]),             "mutation: probability of adding a neuron",},
				{"fS_mut_rem_neuro",         0, 0, "Remove neuron",            "f 0 100 10", FIELD(prob[FS_REM_NEURO]),             "mutation: probability of removing a neuron",},
				{"fS_mut_mod_neuro",        0, 0, "Modify neuron",            "f 0 100 10", FIELD(prob[FS_MOD_NEURO_CONNECTION]), "mutation: probability of changing a neuron connection",},
				{"fS_mut_add_neuro_conn",   0, 0, "Add neuron connection",    "f 0 100 10", FIELD(prob[FS_ADD_NEURO_CONNECTION]), "mutation: probability of adding a neuron connection",},
				{"fS_mut_rem neuro_conn",    0, 0, "Remove neuron connection", "f 0 100 10", FIELD(prob[FS_REM_NEURO_CONNECTION]),  "mutation: probability of removing a neuron connection",},
				{"fS_mut_mod_neuro_params", 0, 0, "Modify neuron params",     "f 0 100 10", FIELD(prob[FS_MOD_NEURO_PARAMS]),     "mutation: probability of changing a neuron param",},
		};

#undef FIELDSTRUCT

fS_Operators::fS_Operators()
{
	par.setParamTab(GENOfSparam_tab);
	par.select(this);
	par.setDefault();
}

int fS_Operators::checkValidity(const char *geno, const char *genoname)
{
	try
	{
		fS_Genotype genotype = fS_Genotype(geno);
	}
	catch (const char *msg)
	{
//		std::cout<<msg<<std::endl;
		logPrintf("fS_Operators", "checkValidity", LOG_ERROR, msg);
		return 1;
	}
	return 0;
}


int fS_Operators::mutate(char *&geno, float &chg, int &method)
{
	fS_Genotype genotype(geno);

	bool result = false;
	method = GenoOperators::roulette(prob, FS_OPCOUNT);
	switch (method)
	{
		case FS_ADD_PART:
			result = genotype.addPart();
			break;
		case FS_REM_PART:
			result = genotype.removePart();
			break;
		case FS_MOD_PART:
			result = genotype.changePartType();
			break;
		case FS_ADD_JOINT:
			result = genotype.addJoint();
			break;
		case FS_REM_JOINT:
			result = genotype.removeJoint();
			break;
		case FS_ADD_PARAM:
			result = genotype.addParam();
			break;
		case FS_REM_PARAM:
			result = genotype.removeParam();
			break;
		case FS_MOD_PARAM:
			result = genotype.changeParam();
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

int fS_Operators::crossOver(char *&g1, char *&g2, float &chg1, float &chg2)
{
	int parentCount = 2;
	fS_Genotype *parents[parentCount] = {new fS_Genotype(g1), new fS_Genotype(g2)};

	if (parents[0]->startNode->childSize == 0 || parents[1]->startNode->childSize == 0)
	{
		delete parents[0];
		delete parents[1];
		return GENOPER_OPFAIL;
	}

	Node *chosen[parentCount];
	int indexes[2];
	for (int i = 0; i < parentCount; i++)
	{
		vector < Node * > allNodes = parents[i]->getAllNodes();
		do
		{
			chosen[i] = allNodes[randomFromRange(allNodes.size(), 0)];
		} while (chosen[i]->childSize == 0);
		indexes[i] = randomFromRange(chosen[i]->childSize, 0);
	}

	double subtreeSize1 = chosen[0]->children[indexes[0]]->getNodeCount();
	double subtreeSize2 = chosen[1]->children[indexes[1]]->getNodeCount();
	double restSize1 = parents[0]->getNodeCount() - subtreeSize1;
	double restSize2 = parents[1]->getNodeCount() - subtreeSize2;

	chg1 = restSize1 / (restSize1 + subtreeSize2);
	chg2 = restSize2 / (restSize2 + subtreeSize1);
	swap(chosen[0]->children[indexes[0]], chosen[1]->children[indexes[1]]);

	free(g1);
	free(g2);
	g1 = strdup(parents[0]->getGeno().c_str());
	g2 = strdup(parents[1]->getGeno().c_str());

	delete parents[0];
	delete parents[1];
	return GENOPER_OK;
}