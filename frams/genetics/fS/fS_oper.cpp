// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 2019-2020  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <float.h>
#include <assert.h>
#include "fS_oper.h"
#include "frams/util/rndutil.h"

#define FIELDSTRUCT GenoOper_fS
static ParamEntry genooper_fS_paramtab[] =
		{
				{"Genetics: fS",            1, FS_OPCOUNT + 5,},
				{"fS_mut_add_part",         0, 0, "Add part",                    "f 0 100 10", FIELD(prob[FS_ADD_PART]),             "mutation: probability of adding a part",},
				{"fS_mut_rem_part",         0, 0, "Remove part",                 "f 0 100 10", FIELD(prob[FS_REM_PART]),             "mutation: probability of deleting a part",},
				{"fS_mut_mod_part",         0, 0, "Modify part",                 "f 0 100 10", FIELD(prob[FS_MOD_PART]),             "mutation: probability of changing the part type",},
				{"fS_mut_change_joint",     0, 0, "Change joint",                "f 0 100 10", FIELD(prob[FS_CHANGE_JOINT]),         "mutation: probability of changing a joint",},
				{"fS_mut_add_param",        0, 0, "Add param",                   "f 0 100 10", FIELD(prob[FS_ADD_PARAM]),            "mutation: probability of adding a parameter",},
				{"fS_mut_rem_param",        0, 0, "Remove param",                "f 0 100 10", FIELD(prob[FS_REM_PARAM]),            "mutation: probability of removing a parameter",},
				{"fS_mut_mod_param",        0, 0, "Modify param",                "f 0 100 10", FIELD(prob[FS_MOD_PARAM]),            "mutation: probability of modifying a parameter",},
				{"fS_mut_mod_mod",          0, 0, "Modify modifier",             "f 0 100 10", FIELD(prob[FS_MOD_MOD]),              "mutation: probability of modifying a modifier",},
				{"fS_mut_add_neuro",        0, 0, "Add neuron",                  "f 0 100 10", FIELD(prob[FS_ADD_NEURO]),            "mutation: probability of adding a neuron",},
				{"fS_mut_rem_neuro",        0, 0, "Remove neuron",               "f 0 100 10", FIELD(prob[FS_REM_NEURO]),            "mutation: probability of removing a neuron",},
				{"fS_mut_mod_neuro_conn",   0, 0, "Modify neuron connection",    "f 0 100 10", FIELD(prob[FS_MOD_NEURO_CONNECTION]), "mutation: probability of changing a neuron connection",},
				{"fS_mut_add_neuro_conn",   0, 0, "Add neuron connection",       "f 0 100 10", FIELD(prob[FS_ADD_NEURO_CONNECTION]), "mutation: probability of adding a neuron connection",},
				{"fS_mut_rem_neuro_conn",   0, 0, "Remove neuron connection",    "f 0 100 10", FIELD(prob[FS_REM_NEURO_CONNECTION]), "mutation: probability of removing a neuron connection",},
				{"fS_mut_mod_neuro_params", 0, 0, "Modify neuron params",        "f 0 100 10", FIELD(prob[FS_MOD_NEURO_PARAMS]),     "mutation: probability of changing a neuron param",},
				{"fS_circle_section",       0, 0, "Ensure circle section",       "d 0 1 1",    FIELD(ensureCircleSection),           "Ensure that ellipsoids and cylinders have circle cross-section"},
				{"fS_use_elli",             0, 0, "Use ellipsoids in mutations", "d 0 1 1",    FIELD(useElli),                       "Use ellipsoids in mutations"},
				{"fS_use_cub",              0, 0, "Use cuboids in mutations",    "d 0 1 1",    FIELD(useCub),                        "Use cuboids in mutations"},
				{"fS_use_cyl",              0, 0, "Use cylinders in mutations",  "d 0 1 1",    FIELD(useCyl),                        "Use cylinders in mutations"},
				{"fS_mut_add_part_strong",  0, 0, "Strong add part mutation",    "d 0 1 1",    FIELD(strongAddPart),                 "Add part mutation will produce more parametrized parts"},
		};

#undef FIELDSTRUCT

GenoOper_fS::GenoOper_fS()
{
	par.setParamTab(genooper_fS_paramtab);
	par.select(this);
	par.setDefault();
	supported_format = 'S';
}

int GenoOper_fS::checkValidity(const char *geno, const char *genoname)
{
	try
	{
		fS_Genotype genotype(geno);
		int errorPosition = genotype.checkValidityOfPartSizes();
		if (errorPosition != 0)
		{
			logPrintf("GenoOper_fS", "checkValidity", LOG_WARN, "Invalid part scale");
			return errorPosition;
		}
	}
	catch (fS_Exception &e)
	{
		logPrintf("GenoOper_fS", "checkValidity", LOG_WARN, e.what());
		return 1 + e.errorPosition;
	}
	return 0;
}


int GenoOper_fS::mutate(char *&geno, float &chg, int &method)
{
	try
	{
		fS_Genotype genotype(geno);

		// Calculate available part types
		vector <Part::Shape> availablePartShapes;
		if (useElli)
			availablePartShapes.push_back(Part::Shape::SHAPE_ELLIPSOID);
		if (useCub)
			availablePartShapes.push_back(Part::Shape::SHAPE_CUBOID);
		if (useCyl)
			availablePartShapes.push_back(Part::Shape::SHAPE_CYLINDER);

		// Select a mutation
		bool result = false;
		method = GenoOperators::roulette(prob, FS_OPCOUNT);
		switch (method)
		{
			case FS_ADD_PART:
				result = addPart(genotype, availablePartShapes);
				break;
			case FS_REM_PART:
				result = removePart(genotype);
				break;
			case FS_MOD_PART:
				result = changePartType(genotype, availablePartShapes);
				break;
			case FS_CHANGE_JOINT:
				result = changeJoint(genotype);
				break;
			case FS_ADD_PARAM:
				result = addParam(genotype);
				break;
			case FS_REM_PARAM:
				result = removeParam(genotype);
				break;
			case FS_MOD_PARAM:
				result = changeParam(genotype);
				break;
			case FS_MOD_MOD:
				result = changeModifier(genotype);
				break;
			case FS_ADD_NEURO:
				result = addNeuro(genotype);
				break;
			case FS_REM_NEURO:
				result = removeNeuro(genotype);
				break;
			case FS_MOD_NEURO_CONNECTION:
				result = changeNeuroConnection(genotype);
				break;
			case FS_ADD_NEURO_CONNECTION:
				result = addNeuroConnection(genotype);
				break;
			case FS_REM_NEURO_CONNECTION:
				result = removeNeuroConnection(genotype);
				break;
			case FS_MOD_NEURO_PARAMS:
				result = changeNeuroParam(genotype);
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
	catch (fS_Exception &e)
	{
		logPrintf("GenoOper_fS", "mutate", LOG_WARN, e.what());
		return GENOPER_OPFAIL;
	}
}

int GenoOper_fS::crossOver(char *&g0, char *&g1, float &chg0, float &chg1)
{
	try
	{
		assert(PARENT_COUNT == 2); // Cross over works only for 2 parents
		fS_Genotype *parents[PARENT_COUNT] = {new fS_Genotype(g0), new fS_Genotype(g1)};

		// Choose random subtrees that have similar size
		Node *selected[PARENT_COUNT];
		vector < Node * > allNodes0 = parents[0]->getAllNodes();
		vector < Node * > allNodes1 = parents[1]->getAllNodes();

		double bestQuotient = DBL_MAX;
		for (int i = 0; i < crossOverTries; i++)
		{
			Node *tmp0 = allNodes0[rndUint(allNodes0.size())];
			Node *tmp1 = allNodes1[rndUint(allNodes1.size())];
			// Choose this pair if it is the most similar
			double quotient = double(tmp0->getNodeCount()) / double(tmp1->getNodeCount());
			if (quotient < 1.0)
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
		for (int i = 0; i < PARENT_COUNT; i++)
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
	}
	catch (fS_Exception &e)
	{
		logPrintf("GenoOper_fS", "crossOver", LOG_WARN, e.what());
		return GENOPER_OPFAIL;
	}
	return GENOPER_OK;
}

const char *GenoOper_fS::getSimplest()
{
	return "1.1,0,0.4:C{x=0.80599;y=0.80599;z=0.80599}";
}

uint32_t GenoOper_fS::style(const char *geno, int pos)
{
	char ch = geno[pos];
	uint32_t style = GENSTYLE_CS(0, GENSTYLE_NONE);
	if (ch == ELLIPSOID || ch == CUBOID || ch == CYLINDER) // part type
	{
		style = GENSTYLE_RGBS(0, 0, 200, GENSTYLE_BOLD);
	} else if (JOINTS.find(ch) != string::npos)    // Joint type
	{
		style = GENSTYLE_RGBS(0, 200, 200, GENSTYLE_BOLD);
	} else if (MODIFIERS.find(ch) != string::npos) // Modifier
	{
		style = GENSTYLE_RGBS(0, 200, 0, GENSTYLE_NONE);
	} else if (isdigit(ch) || strchr(".", ch)) // Numerical value
	{
		style = GENSTYLE_RGBS(200, 0, 0, GENSTYLE_NONE);
	} else if (strchr("()_;[],=", ch))
	{
		style = GENSTYLE_CS(0, GENSTYLE_BOLD); // Important char
	}

	return style;
}

void GenoOper_fS::rearrangeConnectionsBeforeCrossover(fS_Genotype *geno, Node *sub, int &subStart)
{
	vector < fS_Neuron * > genoNeurons = geno->getAllNeurons();
	vector < fS_Neuron * > subNeurons = fS_Genotype::extractNeurons(sub);

	if (!subNeurons.empty())
	{
		subStart = fS_Genotype::getNeuronIndex(genoNeurons, subNeurons[0]);
		fS_Genotype::shiftNeuroConnections(genoNeurons, subStart, subStart + subNeurons.size() - 1, SHIFT::LEFT);
	}
}

void GenoOper_fS::rearrangeConnectionsAfterCrossover(fS_Genotype *geno, Node *sub, int subOldStart)
{
	vector < fS_Neuron * > genoNeurons = geno->getAllNeurons();
	vector < fS_Neuron * > subNeurons = fS_Genotype::extractNeurons(sub);

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

bool GenoOper_fS::addPart(fS_Genotype &geno, const vector <Part::Shape> &availablePartShapes, bool mutateSize)
{
	geno.getState(false);
	Node *node = geno.chooseNode();
	char partShape = SHAPE_TO_GENE.at(availablePartShapes[rndUint(availablePartShapes.size())]);

	Substring substring(&partShape, 0, 1);
	Node *newNode = new Node(substring, node, node->genotypeParams);
	// Add random rotation
	string rotationParams[] {ROT_X, ROT_Y, ROT_Z};
	if (strongAddPart)
	{
		for (int i = 0; i < 3; i++)
			newNode->params[rotationParams[i]] = RndGen.Uni(-M_PI / 2, M_PI / 2);
	} else
	{
		string selectedParam = rotationParams[rndUint(3)];
		newNode->params[selectedParam] = RndGen.Uni(-M_PI / 2, M_PI / 2);
	}
	string rParams[] {RX, RY, RZ};
	if (strongAddPart)
	{
		for (int i = 0; i < 3; i++)
			newNode->params[rParams[i]] = RndGen.Uni(-M_PI / 2, M_PI / 2);
	} else
	{
		string selectedParam = rParams[rndUint(3)];
		newNode->params[selectedParam] = RndGen.Uni(-M_PI / 2, M_PI / 2);
	}
	// Assign part scale to default value
	double volumeMultiplier = pow(node->getParam(SCALE) * node->state->s, 3);
	double minVolume = Model::getMinPart().volume;
	double defVolume = Model::getDefPart().volume * volumeMultiplier;    // Default value after applying modifiers
	double maxVolume = Model::getMaxPart().volume;
	double volume = std::min(maxVolume, std::max(minVolume, defVolume));
	double relativeVolume = volume / volumeMultiplier;    // Volume without applying modifiers

	double newRadius = std::cbrt(relativeVolume / volumeMultipliers.at(newNode->partShape));
	newNode->params[SCALE_X] = newRadius;
	newNode->params[SCALE_Y] = newRadius;
	newNode->params[SCALE_Z] = newRadius;
	node->children.push_back(newNode);

	if (mutateSize)
	{
		geno.getState(false);
		mutateScaleParam(newNode, SCALE_X, true);
		mutateScaleParam(newNode, SCALE_Y, true);
		mutateScaleParam(newNode, SCALE_Z, true);
	}
	return true;
}

bool GenoOper_fS::removePart(fS_Genotype &geno)
{
	Node *randomNode, *selectedChild;
	// Choose a parent with children
	// It may be difficult to choose an eligible node, so the number of tries should be high
	for (int i = 0; i < 10 * mutationTries; i++)
	{
		randomNode = geno.chooseNode();
		int childCount = randomNode->children.size();
		if (childCount > 0)
		{
			int selectedIndex = rndUint(childCount);
			selectedChild = randomNode->children[selectedIndex];
			if (selectedChild->children.empty() && selectedChild->neurons.empty())
			{
				// Remove the selected child
				swap(randomNode->children[selectedIndex], randomNode->children[childCount - 1]);
				randomNode->children.pop_back();
				randomNode->children.shrink_to_fit();
				delete selectedChild;
				return true;
			}
		}
	}
	return false;
}

bool GenoOper_fS::changePartType(fS_Genotype &geno, const vector <Part::Shape> &availablePartShapes)
{
	int availShapesLen = availablePartShapes.size();
	for (int i = 0; i < mutationTries; i++)
	{
		Node *randomNode = geno.chooseNode();
		int index = rndUint(availShapesLen);
		if (availablePartShapes[index] == randomNode->partShape)
			index = (index + 1 + rndUint(availShapesLen - 1)) % availShapesLen;
		Part::Shape newType = availablePartShapes[index];

#ifdef _DEBUG
		if(newType == randomNode->partShape)
			throw fS_Exception("Internal error: invalid part type chosen in mutation.", 1);
#endif

		geno.getState(false);
		double scaleMultiplier = randomNode->getParam(SCALE) * randomNode->state->s;
		double relativeVolume = randomNode->calculateVolume() / pow(scaleMultiplier, 3.0);

		if (!ensureCircleSection || newType == Part::Shape::SHAPE_CUBOID || (randomNode->partShape == Part::Shape::SHAPE_ELLIPSOID && newType == Part::Shape::SHAPE_CYLINDER))
		{
			double radiusQuotient = std::cbrt(volumeMultipliers.at(randomNode->partShape) / volumeMultipliers.at(newType));
			randomNode->params[SCALE_X] = randomNode->getParam(SCALE_X) * radiusQuotient;
			randomNode->params[SCALE_Y] = randomNode->getParam(SCALE_Y) * radiusQuotient;
			randomNode->params[SCALE_Z] = randomNode->getParam(SCALE_Z) * radiusQuotient;
		} else if (randomNode->partShape == Part::Shape::SHAPE_CUBOID && newType == Part::Shape::SHAPE_CYLINDER)
		{
			double newRadius = 0.5 * (randomNode->getParam(SCALE_X) + randomNode->getParam(SCALE_Y));
			randomNode->params[SCALE_X] = 0.5 * relativeVolume / (M_PI * newRadius * newRadius);
			randomNode->params[SCALE_Y] = newRadius;
			randomNode->params[SCALE_Z] = newRadius;
		} else if (newType == Part::Shape::SHAPE_ELLIPSOID)
		{
			double newRelativeRadius = cbrt(relativeVolume / volumeMultipliers.at(newType));
			randomNode->params[SCALE_X] = newRelativeRadius;
			randomNode->params[SCALE_Y] = newRelativeRadius;
			randomNode->params[SCALE_Z] = newRelativeRadius;
		} else
		{
			throw fS_Exception("Invalid part type", 1);
		}
		randomNode->partShape = newType;
		return true;
	}
	return false;
}

bool GenoOper_fS::changeJoint(fS_Genotype &geno)
{
	if (geno.startNode->children.empty())
		return false;

	Node *randomNode = geno.chooseNode(1);        // First part does not have joints
	int jointLen = ALL_JOINTS.length();
	int index = rndUint(jointLen);
	if (ALL_JOINTS[index] == randomNode->joint)
		index = (index + 1 + rndUint(jointLen - 1)) % jointLen;

	randomNode->joint = ALL_JOINTS[index];
	return true;
}

bool GenoOper_fS::addParam(fS_Genotype &geno)
{
	Node *randomNode = geno.chooseNode();
	int paramCount = randomNode->params.size();
	if (paramCount == int(PARAMS.size()))
		return false;
	string key = PARAMS[rndUint(PARAMS.size())];
	if (randomNode->params.count(key) > 0)
		return false;
	// Do not allow invalid changes in part size
	bool isRadiusOfBase = key == SCALE_Y || key == SCALE_Z;
	bool isRadius = isRadiusOfBase || key == SCALE_X;
	if (ensureCircleSection && isRadius)
	{
		if (randomNode->partShape == Part::Shape::SHAPE_ELLIPSOID)
			return false;
		if (randomNode->partShape == Part::Shape::SHAPE_CYLINDER && isRadiusOfBase)
			return false;
	}
	// Add modified default value for param
	randomNode->params[key] = randomNode->defaultValues.at(key);
	geno.getState(false);
	return mutateParamValue(randomNode, key);
}

bool GenoOper_fS::removeParam(fS_Genotype &geno)
{
	// Choose a node with params
	for (int i = 0; i < mutationTries; i++)
	{
		Node *randomNode = geno.chooseNode();
		int paramCount = randomNode->params.size();
		if (paramCount >= 1)
		{
			auto it = randomNode->params.begin();
			advance(it, rndUint(paramCount));
			string key = it->first;
			double value = it->second;

			randomNode->params.erase(key);
			if(geno.checkValidityOfPartSizes() == 0)
				return true;
			else
			{
				randomNode->params[key] = value;
			}
		}
	}
	return false;
}


bool GenoOper_fS::mutateParamValue(Node *node, string key)
{
	// Do not allow invalid changes in part scale
	if (std::find(SCALE_PARAMS.begin(), SCALE_PARAMS.end(), key) == SCALE_PARAMS.end())
	{
		double max = Node::maxValues.at(key);
		double min = Node::minValues.at(key);
		double stddev = (max - min) * node->genotypeParams.paramMutationStrength;
		node->params[key] = GenoOperators::mutateCreep('f', node->getParam(key), min, max, stddev, true);
		return true;
	} else
		return mutateScaleParam(node, key, ensureCircleSection);
}

bool GenoOper_fS::changeParam(fS_Genotype &geno)
{
	geno.getState(false);
	for (int i = 0; i < mutationTries; i++)
	{
		Node *randomNode = geno.chooseNode();
		int paramCount = randomNode->params.size();
		if (paramCount >= 1)
		{
			auto it = randomNode->params.begin();
			advance(it, rndUint(paramCount));
			return mutateParamValue(randomNode, it->first);
		}
	}
	return false;
}

bool GenoOper_fS::changeModifier(fS_Genotype &geno)
{
	Node *randomNode = geno.chooseNode();
	char randomModifier = MODIFIERS[rndUint(MODIFIERS.length())];
	int oldValue = randomNode->modifiers[randomModifier];

	randomNode->modifiers[randomModifier] += rndUint(2) == 0 ? 1 : -1;

	bool isSizeMod = tolower(randomModifier) == SCALE_MODIFIER;
	if (isSizeMod && geno.checkValidityOfPartSizes() != 0)
	{
		randomNode->modifiers[randomModifier] = oldValue;
		return false;
	}
	return true;
}

bool GenoOper_fS::addNeuro(fS_Genotype &geno)
{
	Node *randomNode = geno.chooseNode();
	fS_Neuron *newNeuron;
	NeuroClass *rndclass = GenoOperators::getRandomNeuroClass(Model::SHAPETYPE_SOLIDS);
	if (rndclass->preflocation == NeuroClass::PREFER_JOINT && randomNode == geno.startNode)
		return false;

	const char *name = rndclass->getName().c_str();
	newNeuron = new fS_Neuron(name, randomNode->partDescription->start, strlen(name));
	int effectiveInputCount = rndclass->prefinputs > -1 ? rndclass->prefinputs : 1;
	if (effectiveInputCount > 0)
	{
		// Create as many connections for the neuron as possible (at most prefinputs)
		vector < fS_Neuron * > allNeurons = geno.getAllNeurons();
		vector<int> neuronsWithOutput;
		for (int i = 0; i < int(allNeurons.size()); i++)
		{
			if (allNeurons[i]->getClass()->prefoutput > 0)
				neuronsWithOutput.push_back(i);
		}
		int size = neuronsWithOutput.size();
		if (size > 0)
		{
			for (int i = 0; i < effectiveInputCount; i++)
			{
				int selectedNeuron = neuronsWithOutput[rndUint(size)];
				newNeuron->inputs[selectedNeuron] = DEFAULT_NEURO_CONNECTION_WEIGHT;
			}
		}
	}

	randomNode->neurons.push_back(newNeuron);

	geno.rearrangeNeuronConnections(newNeuron, SHIFT::RIGHT);
	return true;
}

bool GenoOper_fS::removeNeuro(fS_Genotype &geno)
{
	Node *randomNode = geno.chooseNode();
	for (int i = 0; i < mutationTries; i++)
	{
		randomNode = geno.chooseNode();
		if (!randomNode->neurons.empty())
		{
			// Remove the selected neuron
			int size = randomNode->neurons.size();
			fS_Neuron *it = randomNode->neurons[rndUint(size)];
			geno.rearrangeNeuronConnections(it, SHIFT::LEFT);        // Important to rearrange the neurons before deleting
			swap(it, randomNode->neurons.back());
			randomNode->neurons.pop_back();
			randomNode->neurons.shrink_to_fit();
			delete it;
			return true;
		}
	}
	return false;
}

bool GenoOper_fS::changeNeuroConnection(fS_Genotype &geno)
{
	vector < fS_Neuron * > neurons = geno.getAllNeurons();
	if (neurons.empty())
		return false;

	int size = neurons.size();
	for (int i = 0; i < mutationTries; i++)
	{
		fS_Neuron *selectedNeuron = neurons[rndUint(size)];
		if (!selectedNeuron->inputs.empty())
		{
			int inputCount = selectedNeuron->inputs.size();
			auto it = selectedNeuron->inputs.begin();
			advance(it, rndUint(inputCount));

			it->second = GenoOperators::getMutatedNeuronConnectionWeight(it->second);
			return true;
		}
	}
	return false;
}

bool GenoOper_fS::addNeuroConnection(fS_Genotype &geno)
{
	vector < fS_Neuron * > neurons = geno.getAllNeurons();
	if (neurons.empty())
		return false;

	int size = neurons.size();
	fS_Neuron *selectedNeuron;
	for (int i = 0; i < mutationTries; i++)
	{
		selectedNeuron = neurons[rndUint(size)];
		if (selectedNeuron->acceptsInputs())
			break;
	}
	if (!selectedNeuron->acceptsInputs())
		return false;

	for (int i = 0; i < mutationTries; i++)
	{
		int index = rndUint(size);
		if (selectedNeuron->inputs.count(index) == 0 && neurons[index]->getClass()->getPreferredOutput() > 0)
		{

			selectedNeuron->inputs[index] = DEFAULT_NEURO_CONNECTION_WEIGHT;
			return true;
		}
	}
	return false;
}

bool GenoOper_fS::removeNeuroConnection(fS_Genotype &geno)
{
	vector < fS_Neuron * > neurons = geno.getAllNeurons();
	if (neurons.empty())
		return false;

	int size = neurons.size();
	for (int i = 0; i < mutationTries; i++)
	{
		fS_Neuron *selectedNeuron = neurons[rndUint(size)];
		if (!selectedNeuron->inputs.empty())
		{
			int inputCount = selectedNeuron->inputs.size();
			auto it = selectedNeuron->inputs.begin();
			advance(it, rndUint(inputCount));
			selectedNeuron->inputs.erase(it->first);
			return true;
		}
	}
	return false;
}

bool GenoOper_fS::changeNeuroParam(fS_Genotype &geno)
{
	vector < fS_Neuron * > neurons = geno.getAllNeurons();
	if (neurons.empty())
		return false;

	fS_Neuron *neu = neurons[rndUint(neurons.size())];
	return GenoOperators::mutateRandomNeuroClassProperty(neu);
}

bool GenoOper_fS::mutateScaleParam(Node *node, string key, bool ensureCircleSection)
{
	double oldValue = node->getParam(key);
	double volume = node->calculateVolume();
	double valueAtMinVolume, valueAtMaxVolume;
	if(key == SCALE)
	{
		valueAtMinVolume = oldValue * std::cbrt(Model::getMinPart().volume / volume);
		valueAtMaxVolume = oldValue * std::cbrt(Model::getMaxPart().volume / volume);
	}
	else
	{
		valueAtMinVolume = oldValue * Model::getMinPart().volume / volume;
		valueAtMaxVolume = oldValue * Model::getMaxPart().volume / volume;
	}

	double min = std::max(Node::minValues.at(key), valueAtMinVolume);
	double max = std::min(Node::maxValues.at(key), valueAtMaxVolume);
	double stdev = (max - min) * node->genotypeParams.paramMutationStrength;

	node->params[key] = GenoOperators::mutateCreep('f', node->getParam(key), min, max, stdev, true);

	if (!ensureCircleSection || node->isPartScaleValid())
		return true;
	else
	{
		node->params[key] = oldValue;
		return false;
	}
}
