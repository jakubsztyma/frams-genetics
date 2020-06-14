// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 2019-2020  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "fS_general.h"
#include "frams/model/geometry/geometryutils.h"


int fS_Genotype::precision = 2;


/** @name Default values of node parameters*/
const std::map<string, double> defaultParamValues = {
		{INGESTION,      0.25},
		{FRICTION,       0.4},
		{ROT_X,          0.0},
		{ROT_Y,          0.0},
		{ROT_Z,          0.0},
		{RX,             0.0},
		{RY,             0.0},
		{RZ,             0.0},
		{SIZE,           1.0},
		{SIZE_X,         1.0},
		{SIZE_Y,         1.0},
		{SIZE_Z,         1.0},
		{JOINT_DISTANCE, 1.0}
};

double round2(double var)
{
	double value = (int) (var * 100 + .5);
	return (double) value / 100;
}

double fS_stod(const string&  str, size_t* size = 0)
{
	try
	{
		return std::stod(str, size);
	}
	catch(const std::invalid_argument& ex)
	{
		throw fS_Exception("Invalid numeric value");
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
	rotmatrix.rotate(Pt3D(
			Convert::toRadians(rotation.x),
			Convert::toRadians(rotation.y),
			Convert::toRadians(rotation.z)
	));
	vector = rotmatrix.transform(vector);
}

void State::rotate(const Pt3D &rotation)
{
	rotateVector(v, rotation);
	v.normalize();
}

fS_Neuron::fS_Neuron(const char *str, int length)
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
			value = fS_stod(buffer + separatorIndex + 1);
		}
		inputs[fS_stod(buffer, &keyLength)] = value;
	}
}

Node::Node(Substring &restOfGeno, bool _modifierMode, bool _paramMode, bool _cycleMode, bool _isStart = false)
{
	isStart = _isStart;
	modifierMode = _modifierMode;
	paramMode = _paramMode;
	cycleMode = _cycleMode;

	partDescription = new Substring(restOfGeno);

	// TODO decide what causes the problems with part description memory leaks
	try
	{
		extractModifiers(restOfGeno);
		extractPartType(restOfGeno);
		extractNeurons(restOfGeno);
		extractParams(restOfGeno);
	}
	catch (fS_Exception &e)
	{
		delete partDescription;
		throw e;
	}


	partDescription->shortenBy(restOfGeno.len);
	if (restOfGeno.len > 0)
		getChildren(restOfGeno);
}

Node::~Node()
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
		throw fS_Exception("Part type missing");

	for (int i = 0; i < partTypePosition; i++)
	{
		// Extract modifiers and joint
		char mType = restOfGenotype.at(i);
		if (JOINTS.find(mType) != string::npos)
			joint = mType;
		else if (MODIFIERS.find(tolower(mType)) != string::npos)
			modifiers.push_back(mType);
		else
			throw fS_Exception("Invalid modifier");
	}
	restOfGenotype.startFrom(partTypePosition);
}

void Node::extractPartType(Substring &restOfGenotype)
{
	auto itr = GENE_TO_SHAPETYPE.find(restOfGenotype.at(0));
	if (itr == GENE_TO_SHAPETYPE.end())
		throw fS_Exception("Invalid part type");

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
		throw fS_Exception("Lacking neuro end sign");

	for (int i = 0; i < int(separators.size()) - 1; i++)
	{
		int start = separators[i] + 1;
		int length = separators[i + 1] - start;
		fS_Neuron *newNeuron = new fS_Neuron(ns + start, length);
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
		throw fS_Exception("Lacking param end sign");
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
			throw fS_Exception("Parameter separator expected");

		// Compute the value of parameter and assign it to the key
		int valueStartIndex = separatorIndex + 1;
		string key(buffer, separatorIndex);
		if(std::find(PARAMS.begin(), PARAMS.end(), key) == PARAMS.end())
			throw fS_Exception("Invalid parameter key");

		const char *val = buffer + valueStartIndex;
		size_t len = length - valueStartIndex;
		params[key] = fS_stod(val, &len);

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
			throw fS_Exception("Internal error; then maximal distance between parts exceeded.");
		if (currentDistance < minDistance)
			throw fS_Exception("Internal error; the minimal distance between parts exceeded.");

	}

	delete[] centersParent;
	delete[] centers;
	return round2(currentDistance);
}

void Node::getState(State *_state, const Pt3D &parentSize)
{
	if (state != nullptr)
		delete state;
	if (isStart)
		state = _state;
	else
		state = new State(_state);


	// Update state by modifiers
	for (int i = 0; i < int(modifiers.size()); i++)
	{
		char mod = modifiers[i];
		double multiplier = isupper(mod) ? MODIFIER_MULTIPLIER : 1.0 / MODIFIER_MULTIPLIER;
		char modLower = tolower(mod);
		if (modLower == MODIFIERS[0])
			state->ing *= multiplier;
		else if (modLower == MODIFIERS[1])
			state->fr *= multiplier;
		else if (modLower == MODIFIERS[2])
			state->s *= multiplier;
	}

	Pt3D size = calculateSize();
	if (!isStart)
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
		children.push_back(new Node(branches[i], modifierMode, paramMode, cycleMode));
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
			throw fS_Exception("The number of branch start signs does not equal the number of branch end signs");
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
			depth -= 1;
	}
	if (depth != 1)    // T
		throw fS_Exception("The number of branch start signs does not equal the number of branch end signs");
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
	double rx = getParam(ROT_X);
	double ry = getParam(ROT_Y);
	double rz = getParam(ROT_Z);
	return Pt3D(rx, ry, rz);
}

Pt3D Node::getRotation()
{
	double rx = getParam(RX);
	double ry = getParam(RY);
	double rz = getParam(RZ);
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
		result += *it;
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
	string geno = genotype.c_str();
	// M - modifier mode, S - standard mode
	size_t modeSeparatorIndex = geno.find(':');
	if (modeSeparatorIndex == string::npos)
		throw fS_Exception("No mode separator");

	string modeStr = geno.substr(0, modeSeparatorIndex).c_str();
	bool modifierMode = modeStr.find(MODIFIER_MODE) != string::npos;
	bool paramMode = modeStr.find(PARAM_MODE) != string::npos;
	bool cycleMode = modeStr.find(CYCLE_MODE) != string::npos;

	int actualGenoStart = modeSeparatorIndex + 1;
	Substring substring(geno.c_str(), actualGenoStart, geno.length() - actualGenoStart);
	startNode = new Node(substring, modifierMode, paramMode, cycleMode, true);
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

Node *fS_Genotype::chooseNode(int fromIndex = 0)
{
	vector<Node*> allNodes = getAllNodes();
	return allNodes[fromIndex + rndUint(allNodes.size() - fromIndex)];
}

int fS_Genotype::getNodeCount()
{
	return startNode->getNodeCount();
}

bool fS_Genotype::allPartSizesValid()
{
	getState();
	vector<Node*> nodes = getAllNodes();
	for (int i = 0; i < int(nodes.size()); i++)
	{
		if (!nodes[i]->isPartSizeValid())
		{
			return false;
		}
	}
	return true;
}

bool fS_Genotype::addJoint()
{
	if (startNode->children.empty())
		return false;

	Node *randomNode;
	for (int i = 0; i < mutationTries; i++)
	{
		char randomJoint = JOINTS[rndUint(JOINT_COUNT)];
		randomNode = chooseNode(1);        // First part does not have joints
		if (randomNode->joint == DEFAULT_JOINT)
		{
			randomNode->joint = randomJoint;
			return true;
		}
	}
	return false;
}


bool fS_Genotype::removeJoint()
{
	// This operator may can lower success rate that others, as it does not work when there is only one node
	if (startNode->children.size() < 1) // Only one node; there are no joints
		return false;

	// Choose a node with joints
	for (int i = 0; i < mutationTries; i++)
	{
		Node *randomNode = chooseNode(1);    // First part does not have joints
		if (randomNode->joint != DEFAULT_JOINT)
		{
			randomNode->joint = DEFAULT_JOINT;
			return true;
		}
	}
	return false;
}


bool fS_Genotype::removeParam()
{
	// Choose a node with params
	for (int i = 0; i < mutationTries; i++)
	{
		Node *randomNode = chooseNode();
		int paramCount = randomNode->params.size();
		if (paramCount >= 1)
		{
			auto it = randomNode->params.begin();
			advance(it, rndUint(paramCount));
			randomNode->params.erase(it->first);
			return true;
		}
	}
	return false;
}

bool fS_Genotype::changeParam(bool ensureCircleSection)
{
	getState();
	for (int i = 0; i < mutationTries; i++)
	{
		Node *randomNode = chooseNode();
		int paramCount = randomNode->params.size();
		if (paramCount >= 1)
		{
			auto it = randomNode->params.begin();
			advance(it, rndUint(paramCount));

			double multiplier = randomParamMultiplier();


			// Do not allow invalid changes in part size
			if (it->first != SIZE_X && it->first != SIZE_Y && it->first != SIZE_Z)
			{
				it->second *= multiplier;
				return true;
			} else
				return randomNode->changeSizeParam(it->first, multiplier, ensureCircleSection);
		}
	}
	return false;
}

bool fS_Genotype::addParam(bool ensureCircleSection)
{
	Node *randomNode = chooseNode();
	int paramCount = randomNode->params.size();
	if (paramCount == int(PARAMS.size()))
		return false;
	string selectedParam = PARAMS[rndUint(PARAMS.size())];
	// Not allow 'j' parameter when the cycle mode is not on
	if (selectedParam == JOINT_DISTANCE && !startNode->cycleMode)
		return false;
	if (randomNode->params.count(selectedParam) > 0)
		return false;
	// Do not allow invalid changes in part size
	bool isRadiusOfBase = selectedParam == SIZE_X || selectedParam == SIZE_Y;
	bool isRadius = isRadiusOfBase || selectedParam == SIZE_Z;
	if (ensureCircleSection && isRadius)
	{
		if (randomNode->partType == Part::Shape::SHAPE_ELLIPSOID)
			return false;
		if (randomNode->partType == Part::Shape::SHAPE_CYLINDER && isRadiusOfBase)
			return false;
	}
	// Add modified default value for param
	randomNode->params[selectedParam] = defaultParamValues.at(selectedParam);
	return true;
}

bool fS_Genotype::removePart()
{
	Node *randomNode, *selectedChild;
	// Choose a parent with children
	for (int i = 0; i < mutationTries; i++)
	{
		randomNode = chooseNode();
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

bool fS_Genotype::addPart(bool ensureCircleSection, string availableTypes,  bool mutateSize)
{
	getState();
	Node *node = chooseNode();
	char partType = availableTypes[rndUint(availableTypes.length())];

	Substring substring(&partType, 0, 1);
	Node *newNode = new Node(substring, node->modifierMode, node->paramMode, node->cycleMode);
	// Add random rotation
	newNode->params[ROT_X] = RndGen.Uni(-90, 90);
	newNode->params[ROT_Y] = RndGen.Uni(-90, 90);
	newNode->params[ROT_Z] = RndGen.Uni(-90, 90);
	// Assign part size to default value
	double volumeMultiplier = pow(node->getParam(SIZE) * node->state->s, 3);
	double minVolume = Model::getMinPart().volume;
	double defVolume = Model::getDefPart().volume * volumeMultiplier;    // Default value after applying modifiers
	double maxVolume = Model::getMaxPart().volume;
	double volume = std::min(maxVolume, std::max(minVolume, defVolume));
	double relativeVolume = volume / volumeMultiplier;    // Volume without applying modifiers

	double newRadius = Node::calculateRadiusFromVolume(newNode->partType, relativeVolume);
	newNode->params[SIZE_X] = newRadius;
	newNode->params[SIZE_Y] = newRadius;
	newNode->params[SIZE_Z] = newRadius;
	node->children.push_back(newNode);

	if (mutateSize)
	{
		getState();
		newNode->changeSizeParam(SIZE_X, randomParamMultiplier(), true);
		newNode->changeSizeParam(SIZE_Y, randomParamMultiplier(), true);
		newNode->changeSizeParam(SIZE_Z, randomParamMultiplier(), true);
	}
	return true;
}

bool fS_Genotype::changePartType(bool ensureCircleSection, string availTypes)
{
	int availTypesLength = availTypes.length();
	for (int i = 0; i < mutationTries; i++)
	{
		Node *randomNode = chooseNode();
		int index = rndUint(availTypesLength);
		if (availTypes[index] == SHAPETYPE_TO_GENE.at(randomNode->partType))
			index = (index + 1 + rndUint(availTypesLength)) % availTypesLength;
		char newTypeChr = availTypes[index];

		auto itr = GENE_TO_SHAPETYPE.find(newTypeChr);
		Part::Shape newType = itr->second;

#ifdef _DEBUG
		if(newType == randomNode->partType)
			throw fS_Exception("Internal error: invalid part type chosen in mutation.");
#endif

		if (ensureCircleSection)
		{
			getState();
			if (randomNode->partType == Part::Shape::SHAPE_CUBOID
				|| (randomNode->partType == Part::Shape::SHAPE_CYLINDER && newType == Part::Shape::SHAPE_ELLIPSOID))
			{
				double sizeMultiplier = randomNode->getParam(SIZE) * randomNode->state->s;
				double relativeVolume = randomNode->calculateVolume() / pow(sizeMultiplier, 3.0);
				double newRelativeRadius = Node::calculateRadiusFromVolume(newType, relativeVolume);
				randomNode->params[SIZE_X] = newRelativeRadius;
				randomNode->params[SIZE_Y] = newRelativeRadius;
				randomNode->params[SIZE_Z] = newRelativeRadius;
			}
		}
		randomNode->partType = newType;
		return true;
	}
	return false;
}

bool fS_Genotype::addModifier()
{
	Node *randomNode = chooseNode();
	char randomModifier = MODIFIERS[rndUint(MODIFIERS.length())];
	if (rndUint(2) == 1)
		randomModifier = toupper(randomModifier);
	randomNode->modifiers.push_back(randomModifier);

	bool isSizeMod = tolower(randomModifier) == SIZE_MODIFIER;
	if (isSizeMod && !allPartSizesValid())
	{
		randomNode->modifiers.pop_back();
		return false;
	}
	return true;
}

bool fS_Genotype::removeModifier()
{
	for (int i = 0; i < mutationTries; i++)
	{
		Node *randomNode = chooseNode();
		if (!(randomNode->modifiers.empty()))
		{
			char oldMod = randomNode->modifiers.back();
			randomNode->modifiers.pop_back();

			bool isSizeMod = tolower(oldMod) == SIZE_MODIFIER;
			if (isSizeMod && !allPartSizesValid())
			{
				randomNode->modifiers.push_back(oldMod);
				return false;
			}
			return true;
		}
	}
	return false;
}


void fS_Genotype::rearrangeNeuronConnections(fS_Neuron *changedNeuron, SHIFT shift)
{
	vector<fS_Neuron*> neurons = getAllNeurons();
	int changedNeuronIndex = getNeuronIndex(neurons, changedNeuron);
	shiftNeuroConnections(neurons, changedNeuronIndex, changedNeuronIndex, shift);
}

bool fS_Genotype::addNeuro()
{
	Node *randomNode = chooseNode();
	fS_Neuron *newNeuron;
	NeuroClass *rndclass = GenoOperators::getRandomNeuroClass(Model::SHAPE_SOLIDS);
	if(rndclass->preflocation == 2 && randomNode == startNode)
		return false;

	const char *name = rndclass->getName().c_str();
	newNeuron = new fS_Neuron(name, strlen(name));
	int effectiveInputCount = rndclass->prefinputs > -1 ? rndclass->prefinputs : 1;
	if (effectiveInputCount > 0)
	{
		// Create as many connections for the neuron as possible (at most prefinputs)
		vector<fS_Neuron*> allNeurons = getAllNeurons();
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

	rearrangeNeuronConnections(newNeuron, SHIFT::RIGHT);
	return true;
}

bool fS_Genotype::removeNeuro()
{
	Node *randomNode = chooseNode();
	for (int i = 0; i < mutationTries; i++)
	{
		randomNode = chooseNode();
		if (!randomNode->neurons.empty())
		{
			// Remove the selected neuron
			int size = randomNode->neurons.size();
			fS_Neuron *it = randomNode->neurons[rndUint(size)];
			rearrangeNeuronConnections(it, SHIFT::LEFT);        // Important to rearrange the neurons before deleting
			swap(it, randomNode->neurons.back());
			randomNode->neurons.pop_back();
			randomNode->neurons.shrink_to_fit();
			delete it;
			return true;
		}
	}
	return false;
}

bool fS_Genotype::changeNeuroConnection()
{
	vector<fS_Neuron*> neurons = getAllNeurons();
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

			it->second *= GenoOperators::mutateNeuProperty(it->second, selectedNeuron, -1);
			return true;
		}
	}
	return false;
}

bool fS_Genotype::addNeuroConnection()
{
	vector<fS_Neuron*> neurons = getAllNeurons();
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

bool fS_Genotype::removeNeuroConnection()
{
	vector<fS_Neuron*> neurons = getAllNeurons();
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

bool fS_Genotype::changeNeuroParam()
{
	vector<fS_Neuron*> neurons = getAllNeurons();
	if (neurons.empty())
		return false;

	fS_Neuron *selectedNeuron = neurons[rndUint(neurons.size())];
	SyntParam par = selectedNeuron->classProperties();
	int propNo = GenoOperators::selectRandomProperty(selectedNeuron);
	if(propNo != -1)
	{
		double oldValue = par.getDouble(propNo % 100);
		double newValue = GenoOperators::mutateNeuProperty(oldValue, selectedNeuron, propNo);
		par.setDouble(propNo % 100, newValue);
		return true;
	}
	return false;
}