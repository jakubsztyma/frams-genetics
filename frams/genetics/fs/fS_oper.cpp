//
// Created by jakub on 19.03.2020.
//

#include "fS_oper.h"


#define FIELDSTRUCT fS_Operators
static ParamEntry GENOfSparam_tab[] =
		{
				{"Genetics: fS",     1, FS_OPCOUNT,},
				{"fS_mut_add",       0, 0, "Add part",        "f 0 100 10", FIELD(prob[FS_ADD]),       "mutation: probability of adding a part",},
				{"fS_mut_rm",        0, 0, "Remove part",     "f 0 100 10", FIELD(prob[FS_RM]),        "mutation: probability of deleting a part",},
				{"fS_mut_mod",       0, 0, "Modify part",     "f 0 100 10", FIELD(prob[FS_MOD]),       "mutation: probability of changing the part type",},
				{"fS_mut_add_joint", 0, 0, "Add joint",       "f 0 100 10", FIELD(prob[FS_ADD_JOINT]), "mutation: probability of adding a joint",},
				{"fS_mut_rm_joint",  0, 0, "Remove joint",    "f 0 100 10", FIELD(prob[FS_RM_JOINT]),  "mutation: probability of removing a joint",},
				{"fS_mut_add_param", 0, 0, "Add param",       "f 0 100 10", FIELD(prob[FS_ADD_PARAM]), "mutation: probability of adding a parameter",},
				{"fS_mut_rm_param",  0, 0, "Remove param",    "f 0 100 10", FIELD(prob[FS_RM_PARAM]),  "mutation: probability of removing a parameter",},
				{"fS_mut_mod_param", 0, 0, "Modify param",    "f 0 100 10", FIELD(prob[FS_MOD_PARAM]), "mutation: probability of modifying a parameter",},
				{"fS_mut_add_mod",   0, 0, "Add modifier",    "f 0 100 10", FIELD(prob[FS_ADD_MOD]),   "mutation: probability of adding a modifier",},
				{"fS_mut_rm_mod",    0, 0, "Remove modifier", "f 0 100 10", FIELD(prob[FS_RM_MOD]),    "mutation: probability of deleting a modifier",},
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
		case FS_ADD:
			result = genotype.addPart();
			break;
		case FS_RM:
			result = genotype.removePart();
			break;
		case FS_MOD:
			result = genotype.changePartType();
			break;
		case FS_ADD_JOINT:
			result = genotype.addJoint();
			break;
		case FS_RM_JOINT:
			result = genotype.removeJoint();
			break;
		case FS_ADD_PARAM:
			result = genotype.addParam();
			break;
		case FS_RM_PARAM:
			result = genotype.removeParam();
			break;
		case FS_MOD_PARAM:
			result = genotype.changeParam();
			break;
		case FS_ADD_MOD:
			result = genotype.addModifier();
			break;
		case FS_RM_MOD:
			result = genotype.removeModifier();
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
	swap(chosen[0]->children[indexes[0]], chosen[1]->children[indexes[1]]);

	free(g1);
	free(g2);
	g1 = strdup(parents[0]->getGeno().c_str());
	g2 = strdup(parents[1]->getGeno().c_str());

	delete parents[0];
	delete parents[1];
	return GENOPER_OK;
}