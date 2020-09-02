// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 2019-2020  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <float.h>
#include "fS_general.h"
#include "frams/model/geometry/geometryutils.h"
#include "frams/genetics/genooperators.h"
#include "common/Convert.h"
#include "frams/util/rndutil.h"
#include "frams/neuro/neurolibrary.h"
#include "../genooperators.h"
#include "common/nonstd_math.h"
#include "part_distance_estimator.h"

int fS_Genotype::precision = 4;
std::map<string, double> Node::minValues;
std::map<string, double> Node::defaultValues;
std::map<string, double> Node::maxValues;

void Node::prepareParams()
{
	if(minValues.empty())
	{
		minValues = {
				{INGESTION, Model::getMinPart().ingest},
				{FRICTION,  Model::getMinPart().friction},
				{ROT_X,     -M_PI},
				{ROT_Y,     -M_PI},
				{ROT_Z,     -M_PI},
				{RX,        -M_PI},
				{RY,        -M_PI},
				{RZ,        -M_PI},
				{SCALE,      0.01},
				{SCALE_X,    Model::getMinPart().scale.x},
				{SCALE_Y,    Model::getMinPart().scale.y},
				{SCALE_Z,    Model::getMinPart().scale.z}
		};
	}

	if(maxValues.empty())
	{
		maxValues = {
				{INGESTION, Model::getMaxPart().ingest},
				{FRICTION,  Model::getMaxPart().friction},
				{ROT_X,     M_PI},
				{ROT_Y,     M_PI},
				{ROT_Z,     M_PI},
				{RX,        M_PI},
				{RY,        M_PI},
				{RZ,        M_PI},
				{SCALE,      100.0},
				{SCALE_X,    Model::getMaxPart().scale.x},
				{SCALE_Y,    Model::getMaxPart().scale.y},
				{SCALE_Z,    Model::getMaxPart().scale.z}
		};
	}
	if(defaultValues.empty())
	{
		defaultValues = {
				{INGESTION, Model::getDefPart().ingest},
				{FRICTION,  Model::getDefPart().friction},
				{ROT_X,     0.0},
				{ROT_Y,     0.0},
				{ROT_Z,     0.0},
				{RX,        0.0},
				{RY,        0.0},
				{RZ,        0.0},
				{SCALE,      1.0},
				{SCALE_X,    Model::getDefPart().scale.x},
				{SCALE_Y,    Model::getDefPart().scale.y},
				{SCALE_Z,    Model::getDefPart().scale.z}
		};
	}
}

double fS_stod(const string&  str, int start, size_t* size)
{
	try
	{
		return std::stod(str, size);
	}
	catch(const std::invalid_argument& ex)
	{
		throw fS_Exception("Invalid numeric value", start);
	}
	catch(const std::out_of_range& ex)
	{
		throw fS_Exception("Invalid numeric value; out of range", start);
	}
}

State::State(State *_state)
{
	location = Pt3D(_state->location);
	v = Pt3D(_state->v);
	fr = _state->fr;
	s = _state->s;
}

State::State(Pt3D _location, Pt3D _v)
{
	location = Pt3D(_location);
	v = Pt3D(_v);
}

void State::addVector(const double length)
{
	location += v * length;
}

void rotateVector(Pt3D &vector, const Pt3D &rotation)
{
	Orient rotmatrix = Orient_1;
	rotmatrix.rotate(rotation);
	vector = rotmatrix.transform(vector);
}

void State::rotate(const Pt3D &rotation)
{
       rotateVector(v, rotation);
       v.normalize();
}


fS_Neuron::fS_Neuron(const char *str, int _start, int length)
{
	start = _start + 1;
	end = start + length;
	if (length == 0)
		return;

	vector<SString> inputStrings;
	strSplit(SString(str, length), NEURON_INTERNAL_SEPARATOR, false, inputStrings);
	if (inputStrings.empty())
		return;

	int inputStart = 0;
	SString details = "N";

	SString tmp = inputStrings[0];
	if(tmp.indexOf(':') != -1)
		tmp = tmp.substr(0, tmp.indexOf(':'));

	if (NeuroLibrary::staticlibrary.findClassIndex(tmp, true) != -1)
	{
		inputStart = 1;
		details = inputStrings[0];
	}
	setDetails(details);

	for (int i = inputStart; i < int(inputStrings.size()); i++)
	{
		SString keyValue = inputStrings[i];
		int separatorIndex = keyValue.indexOf(NEURON_I_W_SEPARATOR);
		const char *buffer = keyValue.c_str();
		size_t keyLength;
		double value;
		if (separatorIndex == -1)
		{
			keyLength = keyValue.length();
			value = DEFAULT_NEURO_CONNECTION_WEIGHT;
		} else
		{
			keyLength = separatorIndex;
			size_t valueLength = keyValue.length() - (separatorIndex);
			value = fS_stod(buffer + separatorIndex + 1, start, &valueLength);
		}
		inputs[fS_stod(buffer, start, &keyLength)] = value;
	}
}

Node::Node(Substring &restOfGeno, Node *_parent, GenotypeParams _genotypeParams)
{
	prepareParams();
	partDescription = new Substring(restOfGeno);
	genotypeParams = _genotypeParams;
	parent = _parent;

	try
	{
		extractModifiers(restOfGeno);
		extractPartType(restOfGeno);
		extractNeurons(restOfGeno);
		extractParams(restOfGeno);

		partDescription->shortenBy(restOfGeno.len);
		if (restOfGeno.len > 0)
			getChildren(restOfGeno);
	}
	catch(fS_Exception &e)
	{
		cleanUp();
		throw e;
	}
}

Node::~Node()
{
	cleanUp();
}

void Node::cleanUp()
{
	delete partDescription;
	if (state != nullptr)
		delete state;
	for (int i = 0; i < int(neurons.size()); i++)
		delete neurons[i];
	for (int i = 0; i < int(children.size()); i++)
		delete children[i];
}

int Node::getPartPosition(Substring &restOfGenotype)
{
	for (int i = 0; i < restOfGenotype.len; i++)
	{
		if (GENE_TO_SHAPE.find(restOfGenotype.at(i)) != GENE_TO_SHAPE.end())
			return i;
	}
	return -1;
}

void Node::extractModifiers(Substring &restOfGenotype)
{
	int partShapePosition = getPartPosition(restOfGenotype);
	if (partShapePosition == -1)
		throw fS_Exception("Part type missing", restOfGenotype.start);

	for (int i = 0; i < partShapePosition; i++)
	{
		// Extract modifiers and joint
		char mType = restOfGenotype.at(i);
		if (JOINTS.find(tolower(mType)) != string::npos)
			joint = tolower(mType);
		else if (MODIFIERS.find(toupper(mType)) != string::npos)
			modifiers[toupper(mType)] += isupper(mType) ? 1 : -1;
		else
			throw fS_Exception("Invalid modifier", restOfGenotype.start + i);
	}
	restOfGenotype.startFrom(partShapePosition);
}

void Node::extractPartType(Substring &restOfGenotype)
{
	auto itr = GENE_TO_SHAPE.find(restOfGenotype.at(0));
	if (itr == GENE_TO_SHAPE.end())
		throw fS_Exception("Invalid part type", restOfGenotype.start);

	partShape = itr->second;
	restOfGenotype.startFrom(1);
}

vector<int> getSeparatorPositions(const char *str, int len, char separator, char endSign, int &endIndex)
{
	endIndex = -1;
	vector<int> separators {-1};
	for (int i = 0; i < len; i++)
	{
		if (str[i] == separator)
			separators.push_back(i);
		else if (str[i] == endSign)
		{
			endIndex = i;
			break;
		}
	}
	separators.push_back(endIndex); // End of string as last separator
	return separators;
}

void Node::extractNeurons(Substring &restOfGenotype)
{
	if (restOfGenotype.len == 0 || restOfGenotype.at(0) != NEURON_START)
		return;

	const char *ns = restOfGenotype.c_str() + 1;
	int neuronsEndIndex;
	vector<int> separators = getSeparatorPositions(ns, restOfGenotype.len, NEURON_SEPARATOR, NEURON_END, neuronsEndIndex);
	if(neuronsEndIndex == -1)
		throw fS_Exception("Lacking neuro end sign", restOfGenotype.start);

	for (int i = 0; i < int(separators.size()) - 1; i++)
	{
		int start = separators[i] + 1;
		int length = separators[i + 1] - start;
		fS_Neuron *newNeuron = new fS_Neuron(ns + start, restOfGenotype.start + start, length);
		neurons.push_back(newNeuron);
	}

	restOfGenotype.startFrom(neuronsEndIndex + 2);
}

void Node::extractParams(Substring &restOfGenotype)
{
	if (restOfGenotype.len == 0 || restOfGenotype.at(0) != PARAM_START)
		return;

	const char *paramString = restOfGenotype.c_str() + 1;

	// Find the indexes of the parameter separators
	int paramsEndIndex;
	vector<int> separators = getSeparatorPositions(paramString, restOfGenotype.len, PARAM_SEPARATOR, PARAM_END, paramsEndIndex);
	if(paramsEndIndex == -1)
		throw fS_Exception("Lacking param end sign", restOfGenotype.start);

	for (int i = 0; i < int(separators.size()) - 1; i++)
	{
		int start = separators[i] + 1;
		int length = separators[i + 1] - start;
		const char *buffer = paramString + start;

		// Find the index of key-value separator
		int separatorIndex = -1;
		for (int i = 0; i < length; i++)
		{
			if (buffer[i] == PARAM_KEY_VALUE_SEPARATOR)
			{
				separatorIndex = i;
				break;
			}
		}
		if (-1 == separatorIndex)
			throw fS_Exception("Parameter separator expected", restOfGenotype.start);

		// Compute the value of parameter and assign it to the key
		int valueStartIndex = separatorIndex + 1;
		string key(buffer, separatorIndex);
		if(std::find(PARAMS.begin(), PARAMS.end(), key) == PARAMS.end())
			throw fS_Exception("Invalid parameter key", restOfGenotype.start + start);

		const char *val = buffer + valueStartIndex;
		size_t len = length - valueStartIndex;
		double value = fS_stod(val, restOfGenotype.start + start + valueStartIndex, &len);
		if((key==SCALE_X || key==SCALE_Y || key==SCALE_Z) && value <= 0.0)
			throw fS_Exception("Invalid value of radius parameter", restOfGenotype.start + start + valueStartIndex);

		params[key] = value;

	}

	restOfGenotype.startFrom(paramsEndIndex + 2);
}

double Node::getParam(const string &key)
{
	auto item = params.find(key);
	if (item != params.end())
		return item->second;

	auto defaultItem = defaultValues.find(key);
	if(defaultItem == defaultValues.end())
		throw fS_Exception("Default value missing", 0);
	return defaultItem->second;
}

double Node::getParam(const string &key, double defaultValue)
{
	auto item = params.find(key);
	if (item != params.end())
		return item->second;
	return defaultValue;
}


void Node::getState(State *_state, bool calculateLocation)
{
	if (state != nullptr)
		delete state;
	if (parent == nullptr)
		state = _state;
	else
		state = new State(_state);


	// Update state by modifiers
	for (auto it = modifiers.begin(); it != modifiers.end(); ++it)
	{
		char mod = it->first;
		double multiplier = pow(genotypeParams.modifierMultiplier, it->second);
		if (mod == MODIFIERS[0])
			state->ing *= multiplier;
		else if (mod == MODIFIERS[1])
			state->fr *= multiplier;
		else if (mod == MODIFIERS[2])
			state->s *= multiplier;
	}

	if (parent != nullptr && calculateLocation)
	{
		// Rotate
		state->rotate(getVectorRotation());

		double distance = calculateDistanceFromParent();
		state->addVector(distance);
	}
	for (int i = 0; i < int(children.size()); i++)
		children[i]->getState(state, calculateLocation);
}

void Node::getChildren(Substring &restOfGenotype)
{
	vector<Substring> branches = getBranches(restOfGenotype);
	for (int i = 0; i < int(branches.size()); i++)
	{
		children.push_back(new Node(branches[i], this, genotypeParams));
	}
}

vector<Substring> Node::getBranches(Substring &restOfGenotype)
{
	vector<Substring> children;
	if (restOfGenotype.at(0) != BRANCH_START)
	{
		children.push_back(restOfGenotype);  // Only one child
		return children;
	}

	int depth = 0;
	int start = 1;
	char c;
	const char *str = restOfGenotype.c_str();
	bool insideSpecifiation = false;	// True when inside parameter or neuron specification
	for (int i = 0; i < restOfGenotype.len; i++)
	{
		if (depth < 0)
			throw fS_Exception("The number of branch start signs does not equal the number of branch end signs", restOfGenotype.start + i);
		c = str[i];
		if (c == BRANCH_START)
			depth++;
		else if (c == PARAM_START || c == NEURON_START)
			insideSpecifiation = true;
		else if (c == PARAM_END || c == NEURON_END)
			insideSpecifiation = false;
		else if (!insideSpecifiation && ((c == BRANCH_SEPARATOR && depth == 1) || i + 1 == restOfGenotype.len))
		{
			Substring substring(restOfGenotype);
			substring.startFrom(start);
			substring.len = i - start;
			children.push_back(substring);
			start = i + 1;
		} else if (c == BRANCH_END)
			depth--;
	}
	if (depth != 1)    // T
		throw fS_Exception("The number of branch start signs does not equal the number of branch end signs", restOfGenotype.start);
	return children;
}

void Node::calculateScale(Pt3D &scale)
{
	double scaleMultiplier = getParam(SCALE) * state->s;
	scale.x = getParam(SCALE_X) * scaleMultiplier;
	scale.y = getParam(SCALE_Y) * scaleMultiplier;
	scale.z = getParam(SCALE_Z) * scaleMultiplier;
}

double Node::calculateVolume()
{
	double result = 0.0;
	Pt3D scale;
	calculateScale(scale);
	double radiiProduct = scale.x * scale.y * scale.z;
	switch (partShape)
	{
		case Part::Shape::SHAPE_CUBOID:
			result = 8.0 * radiiProduct;
			break;
		case Part::Shape::SHAPE_CYLINDER:
			result = 2.0 * M_PI * radiiProduct;
			break;
		case Part::Shape::SHAPE_ELLIPSOID:
			result = (4.0 / 3.0) * M_PI * radiiProduct;
			break;
		default:
			logMessage("fS", "calculateVolume", LOG_ERROR, "Invalid part type");
	}
	return result;
}

bool Node::isPartScaleValid()
{
	Pt3D scale;
	calculateScale(scale);
	double volume = calculateVolume();
	Part_MinMaxDef minP = Model::getMinPart();
	Part_MinMaxDef maxP = Model::getMaxPart();

	if (volume > maxP.volume || minP.volume > volume)
		return false;
	if (scale.x < minP.scale.x || scale.y < minP.scale.y || scale.z < minP.scale.z)
		return false;
	if (scale.x > maxP.scale.x || scale.y > maxP.scale.y || scale.z > maxP.scale.z)
		return false;

	if (partShape == Part::Shape::SHAPE_ELLIPSOID && scale.maxComponentValue() != scale.minComponentValue())
		// When any radius has a different value than the others
		return false;
	if (partShape == Part::Shape::SHAPE_CYLINDER && scale.y != scale.z)
		// If base radii have different values
		return false;
	return true;
}

Pt3D Node::getVectorRotation()
{
	return Pt3D(getParam(ROT_X, 0.0), getParam(ROT_Y, 0.0), getParam(ROT_Z, 0.0));
}

Pt3D Node::getRotation()
{
	Pt3D rotation = Pt3D(getParam(RX, 0.0), getParam(RY, 0.0), getParam(RZ, 0.0));
	if(genotypeParams.turnWithRotation)
		rotation += getVectorRotation();
	return rotation;
}

void Node::buildModel(Model &model, Node *parent)
{
	createPart();
	model.addPart(part);
	if (parent != nullptr)
		addJointsToModel(model, parent);

	for (int i = 0; i < int(neurons.size()); i++)
	{
		Neuro *neuro = new Neuro(*neurons[i]);
		model.addNeuro(neuro);
		neuro->addMapping(MultiRange(IRange(neurons[i]->start, neurons[i]->end)));
		if (neuro->getClass()->preflocation == NeuroClass::PREFER_JOINT && parent != nullptr)
		{
			neuro->attachToJoint(model.getJoint(model.getJointCount() - 1));
		} else
			neuro->attachToPart(part);
	}

	model.checkpoint();
	part->addMapping(partDescription->toMultiRange());

	for (int i = 0; i < int(children.size()); i++)
	{
		Node *child = children[i];
		child->buildModel(model, this);
	}
}

void Node::createPart()
{
	part = new Part(partShape);
	part->p = Pt3D(state->location);

	part->friction = getParam(FRICTION) * state->fr;
	part->ingest = getParam(INGESTION) * state->ing;
	calculateScale(part->scale);
	part->setRot(getRotation());
}

void Node::addJointsToModel(Model &model, Node *parent)
{
	Joint *j = new Joint();
	j->attachToParts(parent->part, part);
	switch (joint)
	{
		case HINGE_X:
			j->shape = Joint::Shape::SHAPE_HINGE_X;
			break;
		case HINGE_XY:
			j->shape = Joint::Shape::SHAPE_HINGE_XY;
			break;
		default:
			j->shape = Joint::Shape::SHAPE_FIXED;
	}
	model.addJoint(j);
	j->addMapping(partDescription->toMultiRange());
}


void Node::getGeno(SString &result)
{
	if (joint != DEFAULT_JOINT)
		result += joint;
	for (auto it = modifiers.begin(); it != modifiers.end(); ++it)
	{
		char mod = it->first;
		int count = it->second;
		if(it->second < 0)
		{
			mod = tolower(mod);
			count = fabs(count);
		}
		result += std::string(count, mod).c_str();
	}
	result += SHAPE_TO_GENE.at(partShape);

	if (!neurons.empty())
	{
		// Add neurons to genotype string
		result += NEURON_START;
		for (int i = 0; i < int(neurons.size()); i++)
		{
			fS_Neuron *n = neurons[i];
			if (i != 0)
				result += NEURON_SEPARATOR;

			result += n->getDetails();
			if (!n->inputs.empty())
				result += NEURON_INTERNAL_SEPARATOR;

			for (auto it = n->inputs.begin(); it != n->inputs.end(); ++it)
			{
				if (it != n->inputs.begin())
					result += NEURON_INTERNAL_SEPARATOR;
				result += SString::valueOf(it->first);
				if (it->second != DEFAULT_NEURO_CONNECTION_WEIGHT)
				{
					result += NEURON_I_W_SEPARATOR;
					result += SString::valueOf(it->second);
				}
			}
		}
		result += NEURON_END;
	}

	if (!params.empty())
	{
		// Add parameters to genotype string
		result += PARAM_START;
		for (auto it = params.begin(); it != params.end(); ++it)
		{
			if (it != params.begin())
				result += PARAM_SEPARATOR;

			result += it->first.c_str();                    // Add parameter key to string
			result += PARAM_KEY_VALUE_SEPARATOR;
			// Round the value to two decimal places and add to string
			result += doubleToString(it->second, fS_Genotype::precision).c_str();
		}
		result += PARAM_END;
	}

	if (children.size() == 1)
		children[0]->getGeno(result);
	else if (children.size() > 1)
	{
		result += BRANCH_START;
		for (int i = 0; i < int(children.size()) - 1; i++)
		{
			children[i]->getGeno(result);
			result += BRANCH_SEPARATOR;
		}
		children.back()->getGeno(result);
		result += BRANCH_END;
	}
}

void Node::getAllNodes(vector<Node *> &allNodes)
{
	allNodes.push_back(this);
	for (int i = 0; i < int(children.size()); i++)
		children[i]->getAllNodes(allNodes);
}

int Node::getNodeCount()
{
	vector<Node*> allNodes;
	getAllNodes(allNodes);
	return allNodes.size();
}

fS_Genotype::fS_Genotype(const string &geno)
{
	try
	{
		GenotypeParams genotypeParams;
		genotypeParams.modifierMultiplier = 1.1;
		genotypeParams.distanceTolerance = 0.1;
		genotypeParams.relativeDensity = 10.0;
		genotypeParams.turnWithRotation = false;
		genotypeParams.paramMutationStrength = 0.4;

		size_t modeSeparatorIndex = geno.find(MODE_SEPARATOR);
		if (modeSeparatorIndex == string::npos)
			throw fS_Exception("Genotype parameters missing", 0);

		std::vector<SString> paramStrings;
		strSplit(SString(geno.c_str(), modeSeparatorIndex), ',', false, paramStrings);

		if(paramStrings.size() >= 1 && paramStrings[0] != "")
		{
			size_t len0 = paramStrings[0].length();
			genotypeParams.modifierMultiplier = fS_stod(paramStrings[0].c_str(), 0, &len0);
		}
		if(paramStrings.size() >= 2 && paramStrings[1] != "")
		{
			genotypeParams.turnWithRotation = bool(atoi(paramStrings[1].c_str()));
		}
		if(paramStrings.size() >= 3 && paramStrings[2] != "")
		{
			size_t len2 = paramStrings[2].length();
			genotypeParams.paramMutationStrength = fS_stod(paramStrings[2].c_str(), 0, &len2);
		}

		int genoStart = modeSeparatorIndex + 1;
		Substring substring(geno.c_str(), genoStart, geno.length() - genoStart);
		startNode = new Node(substring, nullptr, genotypeParams);
		validateNeuroInputs();
	}
	catch (fS_Exception &e)
	{
		delete startNode;
		throw e;
	}
}

fS_Genotype::~fS_Genotype()
{
	delete startNode;
}

void fS_Genotype::getState(bool calculateLocation)
{
	State *initialState = new State(Pt3D(0), Pt3D(1, 0, 0));
	startNode->getState(initialState, calculateLocation);
}

Model fS_Genotype::buildModel(bool using_checkpoints)
{

	Model model;
	model.open(using_checkpoints);

	getState(true);
	startNode->buildModel(model, nullptr);
	buildNeuroConnections(model);

	model.close();
	return model;
}


void fS_Genotype::buildNeuroConnections(Model &model)
{
	// All the neurons are already created in the model
	vector<fS_Neuron*> allNeurons = getAllNeurons();
	for (int i = 0; i < int(allNeurons.size()); i++)
	{
		fS_Neuron *neuron = allNeurons[i];
		Neuro *modelNeuro = model.getNeuro(i);
		for (auto it = neuron->inputs.begin(); it != neuron->inputs.end(); ++it)
		{
			Neuro *inputNeuro = model.getNeuro(it->first);
			modelNeuro->addInput(inputNeuro, it->second);

		}
	}
}

Node *fS_Genotype::getNearestNode(vector<Node *> allNodes, Node *node)
{
	Node *result = nullptr;
	double minDistance = DBL_MAX, distance = DBL_MAX;
	for (int i = 0; i < int(allNodes.size()); i++)
	{
		Node *otherNode = allNodes[i];
		auto v = node->children;
		if (otherNode != node &&
			find(v.begin(), v.end(), otherNode) == v.end())
		{   // Not the same node and not a child
			distance = node->state->location.distanceTo(otherNode->state->location);
			if (distance < minDistance)
			{
				minDistance = distance;
				result = otherNode;
			}
		}
	}
	return result;
}

SString fS_Genotype::getGeno()
{
	SString geno;
	geno.reserve(100);

	GenotypeParams gp = startNode->genotypeParams;
	geno += doubleToString(gp.modifierMultiplier, precision).c_str();
	geno += ",";
	geno += std::to_string(int(gp.turnWithRotation)).c_str();
	geno += ",";
	geno += doubleToString(gp.paramMutationStrength, precision).c_str();
	geno += MODE_SEPARATOR;

	startNode->getGeno(geno);
	return geno;
}

vector<fS_Neuron *> fS_Genotype::extractNeurons(Node *node)
{
	vector<Node*> allNodes;
	node->getAllNodes(allNodes);

	vector<fS_Neuron*> allNeurons;
	for (int i = 0; i < int(allNodes.size()); i++)
	{
		for (int j = 0; j < int(allNodes[i]->neurons.size()); j++)
		{
			allNeurons.push_back(allNodes[i]->neurons[j]);
		}
	}
	return allNeurons;
}

int fS_Genotype::getNeuronIndex(vector<fS_Neuron *> neurons, fS_Neuron *changedNeuron)
{
	int neuronIndex = -1;
	for (int i = 0; i < int(neurons.size()); i++)
	{
		if (changedNeuron == neurons[i])
		{
			neuronIndex = i;
			break;
		}
	}
	return neuronIndex;
}

void fS_Genotype::shiftNeuroConnections(vector<fS_Neuron *> &neurons, int start, int end, SHIFT shift)
{
	if (start == -1 || end == -1)
		return;
	int shiftValue = end - start + 1;
	if (shift == SHIFT::LEFT)
		shiftValue *= -1;

	for (int i = 0; i < int(neurons.size()); i++)
	{
		fS_Neuron *n = neurons[i];
		std::map<int, double> newInputs;
		for (auto it = n->inputs.begin(); it != n->inputs.end(); ++it)
		{
			if (start > it->first)
				newInputs[it->first] = it->second;
			else if (it->first >= start)
			{
				if (end >= it->first)
				{
					if (shift == SHIFT::RIGHT)
						newInputs[it->first + shiftValue] = it->second;
					// If shift == -1, just delete the input
				} else if (it->first > end)
					newInputs[it->first + shiftValue] = it->second;
			}
		}
		n->inputs = newInputs;
	}
}

vector<Node *> fS_Genotype::getAllNodes()
{
	vector<Node*> allNodes;
	startNode->getAllNodes(allNodes);
	return allNodes;
}

vector<fS_Neuron *> fS_Genotype::getAllNeurons()
{
	return extractNeurons(startNode);
}

Node *fS_Genotype::chooseNode(int fromIndex)
{
	vector<Node*> allNodes = getAllNodes();
	return allNodes[fromIndex + rndUint(allNodes.size() - fromIndex)];
}

int fS_Genotype::getNodeCount()
{
	return startNode->getNodeCount();
}

int fS_Genotype::checkValidityOfPartSizes()
{
	getState(false);
	vector<Node*> nodes = getAllNodes();
	for (int i = 0; i < int(nodes.size()); i++)
	{
		if (!nodes[i]->isPartScaleValid())
		{
			return 1 + nodes[i]->partDescription->start;
		}
	}
	return 0;
}


void fS_Genotype::validateNeuroInputs()
{

	// Validate neuro input numbers
	vector<fS_Neuron*> allNeurons = getAllNeurons();
	int allNeuronsSize = allNeurons.size();
	for(int i=0; i<allNeuronsSize; i++)
	{
		fS_Neuron *n = allNeurons[i];
		for (auto it = n->inputs.begin(); it != n->inputs.end(); ++it)
		{
			if (it->first < 0 || it->first >= allNeuronsSize)
				throw fS_Exception("Invalid neuron input", 0);
		}
	}
}


void fS_Genotype::rearrangeNeuronConnections(fS_Neuron *changedNeuron, SHIFT shift)
{
	vector<fS_Neuron*> neurons = getAllNeurons();
	int changedNeuronIndex = getNeuronIndex(neurons, changedNeuron);
	shiftNeuroConnections(neurons, changedNeuronIndex, changedNeuronIndex, shift);
}

double Node::calculateDistanceFromParent()
{
	Pt3D scale;
	calculateScale(scale);
	Pt3D parentScale;
	parent->calculateScale(parentScale);    // Here we are sure that parent is not nullptr
	Part *tmpPart = PartDistanceEstimator::buildTemporaryPart(partShape, scale, getRotation());
	Part *parentTmpPart = PartDistanceEstimator::buildTemporaryPart(parent->partShape, parentScale, parent->getRotation());

	double result;
	tmpPart->p = state->v;
	result = PartDistanceEstimator::calculateDistance(*tmpPart, *parentTmpPart, genotypeParams.distanceTolerance, genotypeParams.relativeDensity);


	delete tmpPart;
	delete parentTmpPart;
	return result;
}