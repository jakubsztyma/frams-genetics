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


int fS_Genotype::precision = 4;
bool fS_Genotype::TURN_WITH_ROTATION = false;


double round2(double var)
{
	double value = (int) (var * 100 + .5);
	return (double) value / 100;
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


fS_Neuron::fS_Neuron(const char *str, int start, int length)
{
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
			keyLength = keyValue.len();
			value = DEFAULT_NEURO_CONNECTION_WEIGHT;
		} else
		{
			keyLength = separatorIndex;
			size_t valueLength = keyValue.len() - (separatorIndex);
			value = fS_stod(buffer + separatorIndex + 1, start, &valueLength);
		}
		inputs[fS_stod(buffer, start, &keyLength)] = value;
	}
}

Node::Node(Substring &restOfGeno, bool _modifierMode, bool _paramMode, bool _cycleMode, Node *_parent)
{
	parent = _parent;
	modifierMode = _modifierMode;
	paramMode = _paramMode;
	cycleMode = _cycleMode;
	partDescription = new Substring(restOfGeno);

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
		if (GENE_TO_SHAPETYPE.find(restOfGenotype.at(i)) != GENE_TO_SHAPETYPE.end())
			return i;
	}
	return -1;
}

void Node::extractModifiers(Substring &restOfGenotype)
{
	int partTypePosition = getPartPosition(restOfGenotype);
	if (partTypePosition == -1)
		throw fS_Exception("Part type missing", restOfGenotype.start);

	for (int i = 0; i < partTypePosition; i++)
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
	restOfGenotype.startFrom(partTypePosition);
}

void Node::extractPartType(Substring &restOfGenotype)
{
	auto itr = GENE_TO_SHAPETYPE.find(restOfGenotype.at(0));
	if (itr == GENE_TO_SHAPETYPE.end())
		throw fS_Exception("Invalid part type", restOfGenotype.start);

	partType = itr->second;
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
		if((key==SIZE_X || key==SIZE_Y || key==SIZE_Z) && value <= 0.0)
			throw fS_Exception("Invalid value of radius parameter", restOfGenotype.start + start + valueStartIndex);

		params[key] = value;

	}

	restOfGenotype.startFrom(paramsEndIndex + 2);
}

double Node::getParam(string key)
{
	auto item = params.find(key);
	if (item != params.end())
		return item->second;
	else
		return defaultParamValues.at(key);
}

double avg(double a, double b)
{
	return 0.5 * (a + b);
}

double min3(Pt3D p)
{
	double tmp = p.x;
	if (p.y < tmp)
		tmp = p.y;
	if (p.z < tmp)
		tmp = p.z;
	return tmp;
}

double max3(Pt3D p)
{
	double tmp = p.x;
	if (p.y > tmp)
		tmp = p.y;
	if (p.z > tmp)
		tmp = p.z;
	return tmp;
}

double getSphereCoordinate(double dimension, double sphereDiameter, double index, int count)
{
	if (count == 1)
		return 0;
	return (dimension - sphereDiameter) * (index / (count - 1) - 0.5);
}

Pt3D *findSphereCenters(int &sphereCount, double &sphereRadius, Pt3D radii, Pt3D rotations)
{
	double sphereRelativeDistance = SPHERE_RELATIVE_DISTANCE;
	double minRadius = min3(radii);
	if(minRadius <= 0)
	    throw fS_Exception("Invalid part size", 0);
	double maxRadius = max3(radii);
	if (MAX_DIAMETER_QUOTIENT > maxRadius / minRadius)
		sphereRadius = minRadius;
	else
	{
		// When max radius is much bigger than min radius
		sphereRelativeDistance = 1.0;   // Make the spheres adjacent to speed up the computation
		sphereRadius = maxRadius / MAX_DIAMETER_QUOTIENT;
	}
	double sphereDiameter = 2 * sphereRadius;

	double *diameters = new double[3] {2 * radii.x, 2 * radii.y, 2 * radii.z};
	int counts[3];
	for (int i = 0; i < 3; i++)
	{
		counts[i] = 1;
		if (diameters[i] > sphereDiameter)
			counts[i] += ceil((diameters[i] - sphereDiameter) / sphereDiameter / sphereRelativeDistance);
	}

	sphereCount = counts[0] * counts[1] * counts[2];
	double x, y, z;
	int totalCount = 0;
	Pt3D *centers = new Pt3D[sphereCount];
	for (double xi = 0; xi < counts[0]; xi++)
	{
		x = getSphereCoordinate(diameters[0], sphereDiameter, xi, counts[0]);
		for (double yi = 0; yi < counts[1]; yi++)
		{
			y = getSphereCoordinate(diameters[1], sphereDiameter, yi, counts[1]);
			for (double zi = 0; zi < counts[2]; zi++)
			{
				z = getSphereCoordinate(diameters[2], sphereDiameter, zi, counts[2]);
				centers[totalCount] = Pt3D(x, y, z);
				rotateVector(centers[totalCount], rotations);
				totalCount++;
			}
		}
	}
	delete[] diameters;
	return centers;
}

int isCollision(Pt3D *centersParent, Pt3D *centers, int parentSphereCount, int sphereCount, Pt3D &vector,
				double distanceThreshold)
{
	double upperThreshold = distanceThreshold;
	double lowerThreshold = SPHERE_DISTANCE_TOLERANCE * distanceThreshold;
	double distance;
	double dx, dy, dz;
	bool existsAdjacent = false;
	Pt3D *tmpPoint;
	for (int sc = 0; sc < sphereCount; sc++)
	{
		Pt3D shiftedSphere = Pt3D(centers[sc]);
		shiftedSphere += vector;
		for (int psc = 0; psc < parentSphereCount; psc++)
		{
			tmpPoint = &centersParent[psc];
			dx = shiftedSphere.x - tmpPoint->x;
			dy = shiftedSphere.y - tmpPoint->y;
			dz = shiftedSphere.z - tmpPoint->z;
			distance = sqrt(dx * dx + dy * dy + dz * dz);

			if (distance <= upperThreshold)
			{
				if (distance >= lowerThreshold)
					existsAdjacent = true;
				else
				{
					return COLLISION;
				}
			}
		}
	}
	if (existsAdjacent)
		return ADJACENT;
	else
		return DISJOINT;
}

double getDistance(Pt3D radiiParent, Pt3D radii, Pt3D vector, Pt3D rotationParent, Pt3D rotation)
{
	int parentSphereCount, sphereCount;
	double parentSphereRadius, sphereRadius;
	Pt3D *centersParent = findSphereCenters(parentSphereCount, parentSphereRadius, radiiParent, rotationParent);
	Pt3D *centers = findSphereCenters(sphereCount, sphereRadius, radii, rotation);

	double distanceThreshold = sphereRadius + parentSphereRadius;
	double minDistance = 0.0;
	double maxDistance = 2 * (max3(radiiParent) + max3(radii));
	double currentDistance = avg(maxDistance, minDistance);
	int result = -1;
	while (result != ADJACENT)
	{
		Pt3D currentVector = vector * currentDistance;
		result = isCollision(centersParent, centers, parentSphereCount, sphereCount, currentVector, distanceThreshold);
		if (result == DISJOINT)
		{
			maxDistance = currentDistance;
			currentDistance = avg(currentDistance, minDistance);
		} else if (result == COLLISION)
		{
			minDistance = currentDistance;
			currentDistance = avg(maxDistance, currentDistance);
		}
		if (currentDistance > maxDistance)
			throw fS_Exception("Internal error; then maximal distance between parts exceeded.", 0);
		if (currentDistance < minDistance)
			throw fS_Exception("Internal error; the minimal distance between parts exceeded.", 0);

	}

	delete[] centersParent;
	delete[] centers;
	return round2(currentDistance);
}

void Node::getState(State *_state, const Pt3D &parentSize)
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
		double multiplier = pow(MODIFIER_MULTIPLIER, it->second);
		if (mod == MODIFIERS[0])
			state->ing *= multiplier;
		else if (mod == MODIFIERS[1])
			state->fr *= multiplier;
		else if (mod == MODIFIERS[2])
			state->s *= multiplier;
	}

	Pt3D size = calculateSize();
	if (parent != nullptr)
	{
		// Rotate
		state->rotate(getVectorRotation());

		double distance = getDistance(parentSize, size, state->v, getRotation(), getRotation());
		state->addVector(distance);
	}
	for (int i = 0; i < int(children.size()); i++)
		children[i]->getState(state, size);
}

void Node::getChildren(Substring &restOfGenotype)
{
	vector<Substring> branches = getBranches(restOfGenotype);
	for (int i = 0; i < int(branches.size()); i++)
	{
		children.push_back(new Node(branches[i], modifierMode, paramMode, cycleMode, this));
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
	for (int i = 0; i < restOfGenotype.len; i++)
	{
		if (depth < 0)
			throw fS_Exception("The number of branch start signs does not equal the number of branch end signs", restOfGenotype.start + i);
		c = str[i];
		if (c == BRANCH_START)
			depth++;
		else if ((c == BRANCH_SEPARATOR && depth == 1) || i + 1 == restOfGenotype.len)
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

Pt3D Node::calculateSize()
{
	double sizeMultiplier = getParam(SIZE) * state->s;
	double sx = getParam(SIZE_X) * sizeMultiplier;
	double sy = getParam(SIZE_Y) * sizeMultiplier;
	double sz = getParam(SIZE_Z) * sizeMultiplier;
	return Pt3D(sx, sy, sz);
}

double Node::calculateVolume()
{
	double result;
	Pt3D size = calculateSize();
	double radiiProduct = size.x * size.y * size.z;
	switch (partType)
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

bool Node::isPartSizeValid()
{
	Pt3D size = calculateSize();
	double volume = calculateVolume();
	Part_MinMaxDef minP = Model::getMinPart();
	Part_MinMaxDef maxP = Model::getMaxPart();

	if (volume > maxP.volume || minP.volume > volume)
		return false;
	if (size.x < minP.scale.x || size.y < minP.scale.y || size.z < minP.scale.z)
		return false;
	if (size.x > maxP.scale.x || size.y > maxP.scale.y || size.z > maxP.scale.z)
		return false;

	if (partType == Part::Shape::SHAPE_ELLIPSOID && max3(size) != min3(size))
		// When not all radii have different values
		return false;
	if (partType == Part::Shape::SHAPE_CYLINDER && size.x != size.y)
		// If base radii have different values
		return false;
	return true;
}

bool Node::hasPartSizeParam()
{
	return params.count(SIZE_X) > 0 || params.count(SIZE_Y) > 0 || params.count(SIZE_Z) > 0;
}

Pt3D Node::getVectorRotation()
{
	double rx = Convert::toRadians(getParam(ROT_X));
	double ry = Convert::toRadians(getParam(ROT_Y));
	double rz = Convert::toRadians(getParam(ROT_Z));
	return Pt3D(rx, ry, rz);
}

Pt3D Node::getRotation()
{
	double rx = Convert::toRadians(getParam(RX));
	double ry = Convert::toRadians(getParam(RY));
	double rz = Convert::toRadians(getParam(RZ));
	return Pt3D(rx, ry, rz);
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
		if (neuro->getClass()->preflocation == 2 && parent != nullptr)
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
	part = new Part(partType);
	part->p = Pt3D(round2(state->location.x),
				   round2(state->location.y),
				   round2(state->location.z));

	part->friction = round2(getParam(FRICTION) * state->fr);
	part->ingest = round2(getParam(INGESTION) * state->ing);
	Pt3D size = calculateSize();
	part->scale.x = round2(size.x);
	part->scale.y = round2(size.y);
	part->scale.z = round2(size.z);
	Pt3D rotation = getRotation();
	if(fS_Genotype::TURN_WITH_ROTATION)
		rotation += getVectorRotation();
	part->setRot(rotation);
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
	result += SHAPETYPE_TO_GENE.at(partType);

	if (!neurons.empty())
	{
		// Add neurons to genotype string
		result += NEURON_START;
		for (int i = 0; i < int(neurons.size()); i++)
		{
			fS_Neuron *n = neurons[i];
			if (i != 0)
				result += NEURON_SEPARATOR;
			if (n->getClassName() != "N")
			{
				result += n->getDetails();
				if (!n->inputs.empty())
					result += NEURON_INTERNAL_SEPARATOR;
			}
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
			string value_text = std::to_string(it->second);
			// Round the value to two decimal places and add to string
			result += value_text.substr(0, value_text.find(".") + fS_Genotype::precision).c_str();
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


bool Node::changeSizeParam(string paramKey, double multiplier, bool ensureCircleSection)
{
	double oldValue = getParam(paramKey);
	params[paramKey] = oldValue * multiplier;
	if (!ensureCircleSection || isPartSizeValid())
		return true;
	else
	{
		params[paramKey] = oldValue;
		return false;
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

fS_Genotype::fS_Genotype(const string &genotype)
{
	try
	{
		string geno = genotype.c_str();
		// M - modifier mode, S - standard mode
		size_t modeSeparatorIndex = geno.find(':');
		if (modeSeparatorIndex == string::npos)
			throw fS_Exception("No mode separator", 0);

		string modeStr = geno.substr(0, modeSeparatorIndex).c_str();
		bool modifierMode = modeStr.find(MODIFIER_MODE) != string::npos;
		bool paramMode = modeStr.find(PARAM_MODE) != string::npos;
		bool cycleMode = modeStr.find(CYCLE_MODE) != string::npos;

		int actualGenoStart = modeSeparatorIndex + 1;
		Substring substring(geno.c_str(), actualGenoStart, geno.length() - actualGenoStart);
		startNode = new Node(substring, modifierMode, paramMode, cycleMode, nullptr);
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

void fS_Genotype::getState()
{
	State *initialState = new State(Pt3D(0), Pt3D(1, 0, 0));
	startNode->getState(initialState, Pt3D(1.0));
}

double fS_Genotype::randomParamMultiplier()
{
	double multiplier = 1 + fabs(RndGen.GaussStd());
	if (multiplier > PARAM_MAX_MULTIPLIER)
		multiplier = PARAM_MAX_MULTIPLIER;
	if (rndUint(2) == 0)
		multiplier = 1.0 / multiplier;
	return multiplier;
}

void fS_Genotype::buildModel(Model &model)
{
	getState();
	startNode->buildModel(model, nullptr);

	buildNeuroConnections(model);

	// Additional joints
	vector<Node*> allNodes = getAllNodes();
	for (int i = 0; i < int(allNodes.size()); i++)
	{
		Node *node = allNodes[i];
		if (node->params.find(JOINT_DISTANCE) != node->params.end())
		{
			Node *otherNode = getNearestNode(allNodes, node);
			if (otherNode != nullptr)
			{
				// If other node is close enough, add a joint
				double distance = node->state->location.distanceTo(otherNode->state->location);
				if (distance < node->params[JOINT_DISTANCE])
				{
					Joint *joint = new Joint();
					joint->attachToParts(node->part, otherNode->part);

					joint->shape = Joint::Shape::SHAPE_FIXED;
					model.addJoint(joint);
				}
			}
		}
	}
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
	geno.memoryHint(100);     // Provide a small buffer from the start to improve performance

	if (startNode->modifierMode)
		geno += MODIFIER_MODE;
	if (startNode->paramMode)
		geno += PARAM_MODE;
	if (startNode->cycleMode)
		geno += CYCLE_MODE;

	geno += ':';
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
	getState();
	vector<Node*> nodes = getAllNodes();
	for (int i = 0; i < int(nodes.size()); i++)
	{
		if (!nodes[i]->isPartSizeValid())
		{
			return nodes[i]->partDescription->start;
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

