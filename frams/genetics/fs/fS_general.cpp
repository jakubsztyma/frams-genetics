//
// Created by jakub on 21.02.2020.
//

#include "fS_general.h"
#include "frams/model/geometry/geometryutils.h"


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

State::State(State *_state)
{
	location = Pt3D(_state->location);
	v = Pt3D(_state->v);
	fr = _state->fr;
	sx = _state->sx;
	sy = _state->sy;
	sz = _state->sz;
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

Neuron::Neuron(char neuronType)
{
	cls += neuronType;
	if (NeuroLibrary::staticlibrary.findClassIndex(cls, true) == -1)
		throw "Invalid neuron type";
}

Neuron::Neuron(const char *str, int length)
{
	if (length == 0)
		return;

	int index = 0;
	SString _cls;
	_cls += str[0];
	if (NeuroLibrary::staticlibrary.findClassIndex(_cls, true) != -1)
	{
		cls = _cls;
		index = 1;
		if (length == 1)
			return;
	}

	vector<SString> inputStrings;
	strSplit(SString(str + index, length), NEURON_INPUT_SEPARATOR, false, inputStrings);

	for (unsigned int i = 0; i < inputStrings.size(); i++)
	{
		SString keyValue = inputStrings[i];
		int separatorIndex = keyValue.indexOf(NEURON_I_W_SEPARATOR);
		const char *buffer = keyValue.c_str();
		size_t keyLength;
		double value;
		if (-1 == separatorIndex)
		{
			keyLength = keyValue.len();
			value = DEFAULT_NEURO_CONNECTION_WEIGHT;
		} else
		{
			keyLength = separatorIndex;
			value = std::stod(buffer + separatorIndex + 1);
		}
		inputs[std::stod(buffer, &keyLength)] = value;
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
	catch(const char* msg)
	{
		delete partDescription;
		throw msg;
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
	for(unsigned int i=0; i<neurons.size(); i++)
		delete neurons[i];
	for (unsigned int i = 0; i < childSize; i++)
		delete children[i];
}

int Node::getPartPosition(Substring &restOfGenotype)
{
	for (int i = 0; i < restOfGenotype.len; i++)
	{
		if (PART_TYPES.find(restOfGenotype.at(i)) != string::npos)
			return i;
	}
	return -1;
}

void Node::extractModifiers(Substring &restOfGenotype)
{
	int partTypePosition = getPartPosition(restOfGenotype);
	if (partTypePosition == -1)
		throw "Part type missing";

	for (int i = 0; i < partTypePosition; i++)
	{
		// Extract modifiers and joints
		char mType = restOfGenotype.at(i);
		if (JOINTS.find(mType) != string::npos)
			joints.insert(mType);
		else if (MODIFIERS.find(tolower(mType)) != string::npos)
			modifiers.push_back(mType);
		else
			throw "Invalid modifier";
	}
	restOfGenotype.startFrom(partTypePosition);
}

void Node::extractPartType(Substring &restOfGenotype)
{
	partType = restOfGenotype.at(0);
	if (PART_TYPES.find(partType) == string::npos)
		throw "Invalid part type";
	restOfGenotype.startFrom(1);
}

vector<int> getSeparatorPositions(const char *str, int len, char separator, char endSign, int &endIndex)
{
	vector<int> separators{-1};
	for(int i=0; i<len; i++)
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

	for (unsigned int i = 0; i < separators.size() - 1; i++)
	{
		int start = separators[i] + 1;
		int length = separators[i+1] - start;
		Neuron *newNeuron = new Neuron(ns + start, length);
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
	for (unsigned int i = 0; i < separators.size() - 1; i++)
	{
		int start = separators[i] + 1;
		int length = separators[i+1] - start;
		const char *buffer = paramString + start;

		// Find the index of key-value separator
		int separatorIndex = -1;
		for(int i=0; i<length; i++)
		{
			if(buffer[i] == PARAM_KEY_VALUE_SEPARATOR)
			{
				separatorIndex = i;
				break;
			}
		}
		if (-1 == separatorIndex)
			throw "Parameter separator expected";

		// Compute the value of parameter and assign it to the key
		int valueStartIndex = separatorIndex + 1;
		string key(buffer,  separatorIndex);
		const char *val = buffer + valueStartIndex;
		size_t len = length - valueStartIndex;
		params[key] = std::stod(val, &len);

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
			throw "Internal error; then maximal distance between parts exceeded.";
		if (currentDistance < minDistance)
			throw "Internal error; the minimal distance between parts exceeded.";

	}

	delete[] centersParent;
	delete[] centers;
	return round2(currentDistance);
}

void Node::getState(State *_state, Pt3D parentSize)
{
	if (isStart)
		state = _state;
	else
		state = new State(_state);


	// Update state by modifiers
	for (unsigned int i = 0; i < modifiers.size(); i++)
	{
		char mod = modifiers[i];
		double multiplier = isupper(mod) ? MODIFIER_MULTIPLIER : 1.0 / MODIFIER_MULTIPLIER;
		char modLower = tolower(mod);
		if (modLower == MODIFIERS[0])
			state->ing *= multiplier;
		else if (modLower == MODIFIERS[1])
			state->fr *= multiplier;
		else if (modLower == MODIFIERS[2])
			state->sx *= multiplier;
		else if (modLower == MODIFIERS[3])
			state->sy *= multiplier;
		else if (modLower == MODIFIERS[4])
			state->sz *= multiplier;
	}

	if (!isStart)
	{
		// Rotate
		Pt3D size = getSize();
		state->rotate(getVectorRotation());

		double distance = getDistance(parentSize, size, state->v, getRotation(), getRotation());
		state->addVector(distance);
	}
}

void Node::getChildren(Substring &restOfGenotype)
{
	vector <Substring> branches = getBranches(restOfGenotype);
	childSize = branches.size();
	for (unsigned int i = 0; i < childSize; i++)
	{
		children.push_back(new Node(branches[i], modifierMode, paramMode, cycleMode));
	}
}

vector <Substring> Node::getBranches(Substring &restOfGenotype)
{
	vector <Substring> children;
	if (restOfGenotype.at(0) != BRANCH_START)
	{
		children.push_back(restOfGenotype);  // Only one child
		return children;
	}
	// TODO raise and error in case of wrong syntax

	int depth = 0;
	int start = 1;
	char c;
	const char *str = restOfGenotype.c_str();
	for (int i = 0; i < restOfGenotype.len; i++)
	{
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
	return children;
}

Pt3D Node::getSize()
{
	double sx = getParam(SIZE_X) * state->sx;
	double sy = getParam(SIZE_Y) * state->sy;
	double sz = getParam(SIZE_Z) * state->sz;
	return Pt3D(sx, sy, sz);
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
	if(parent != nullptr)
		addJointsToModel(model, parent);


	for (unsigned int i = 0; i < neurons.size(); i++)
	{
		Neuron* n = neurons[i];
		Neuro *neuro = model.addNewNeuro();
		if (n->cls != SString())
		{
			SString details;
			details += n->cls;
			neuro->setDetails(details);
		}
		if(neuro->getClass()->preflocation == 2)
		{
			if(parent != nullptr)
				neuro->attachToJoint(model.getJoint(model.getJointCount() - 1));
			else
				neuro->attachToPart(part);
		}
		else
			neuro->attachToPart(part);
	}

	model.checkpoint();
	part->addMapping(partDescription->toMultiRange());

	for (unsigned int i = 0; i < childSize; i++)
	{
		Node *child = children[i];
		child->getState(state, getSize());
		child->buildModel(model, this);
	}
}

void Node::createPart()
{
	Part::Shape model_partType;
	if (partType == PART_TYPES[0])
		model_partType = Part::Shape::SHAPE_ELLIPSOID;
	else if (partType == PART_TYPES[1])
		model_partType = Part::Shape::SHAPE_CUBOID;
	else if (partType == PART_TYPES[2])
		model_partType = Part::Shape::SHAPE_CYLINDER;
	else
		throw "Invalid part type";

	part = new Part(model_partType);
	part->p = Pt3D(round2(state->location.x),
				   round2(state->location.y),
				   round2(state->location.z));

	part->friction = round2(getParam(FRICTION) * state->fr);
	part->ingest = round2(getParam(INGESTION) * state->ing);
	Pt3D size = getSize();
	part->scale.x = round2(size.x);
	part->scale.y = round2(size.y);
	part->scale.z = round2(size.z);
	part->setRot(getRotation());
}

void Node::addJointsToModel(Model &model, Node *parent)
{
	if (joints.empty())
	{
		Joint *joint = new Joint();
		joint->shape = Joint::Shape::SHAPE_FIXED;
		joint->attachToParts(parent->part, part);
		model.addJoint(joint);

		joint->addMapping(partDescription->toMultiRange());
	} else
	{
		for (auto it = joints.begin(); it != joints.end(); ++it)
		{
			Joint *joint = new Joint();
			joint->attachToParts(parent->part, part);
			switch (*it)
			{
				case HINGE_X:
					joint->shape = Joint::Shape::SHAPE_HINGE_X;
					break;
				case HINGE_XY:
					joint->shape = Joint::Shape::SHAPE_HINGE_XY;
					break;
			}
			model.addJoint(joint);
		}
	}
}


void Node::getGeno(SString &result)
{
	for (auto it = joints.begin(); it != joints.end(); ++it)
		result += *it;
	for (auto it = modifiers.begin(); it != modifiers.end(); ++it)
		result += *it;
	result += partType;

	if (!neurons.empty())
	{
		// Add neurons to genotype string
		result += NEURON_START;
		for (unsigned int i = 0; i < neurons.size(); i++)
		{
			Neuron *n = neurons[i];
			if (i != 0)
				result += NEURON_SEPARATOR;
			if (n->cls != SString())
				result += n->cls;
			for (auto it = n->inputs.begin(); it != n->inputs.end(); ++it)
			{
				if (it != n->inputs.begin())
					result += NEURON_INPUT_SEPARATOR;
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
			result += value_text.substr(0, value_text.find(".") + 2).c_str();
		}
		result += PARAM_END;
	}

	if (childSize == 1)
		children[0]->getGeno(result);
	else if (childSize > 1)
	{
		result += BRANCH_START;
		for (unsigned int i = 0; i < childSize - 1; i++)
		{
			children[i]->getGeno(result);
			result += BRANCH_SEPARATOR;
		}
		children[childSize - 1]->getGeno(result);
		result += BRANCH_END;
	}
}

void Node::getAllNodes(vector<Node *> &allNodes)
{
	allNodes.push_back(this);
	for (unsigned int i = 0; i < childSize; i++)
		children[i]->getAllNodes(allNodes);
}

int Node::getNodeCount()
{
	vector<Node*> allNodes;
	getAllNodes(allNodes);
	return allNodes.size();
}

fS_Genotype::fS_Genotype(const SString &genotype)
{
	// M - modifier mode, S - standard mode
	int modeSeparatorIndex = genotype.indexOf(':');
	if (modeSeparatorIndex == -1)
		throw "No mode separator";

	SString modeStr = genotype.substr(0, modeSeparatorIndex);
	bool modifierMode = -1 != modeStr.indexOf(MODIFIER_MODE);
	bool paramMode = -1 != modeStr.indexOf(PARAM_MODE);
	bool cycleMode = -1 != modeStr.indexOf(CYCLE_MODE);

	int actualGenoStart = modeSeparatorIndex + 1;
	Substring substring(genotype.c_str(), actualGenoStart, genotype.len() - actualGenoStart);
	startNode = new Node(substring, modifierMode, paramMode, cycleMode, true);
}

fS_Genotype::~fS_Genotype()
{
	delete startNode;
}

void fS_Genotype::buildModel(Model &model)
{
	State *initialState = new State(Pt3D(0), Pt3D(1, 0, 0));
	startNode->getState(initialState, Pt3D(1.0));
	startNode->buildModel(model, nullptr);

	buildNeuroConnections(model);

	// Additional joints
	vector<Node*> allNodes = getAllNodes();
	for (unsigned int i = 0; i < allNodes.size(); i++)
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
	vector < Neuron * > allNeurons = getAllNeurons();
	for (unsigned int i = 0; i < allNeurons.size(); i++)
	{
		Neuron *neuron = allNeurons[i];
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
	for (unsigned int i = 0; i < allNodes.size(); i++)
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


char getRandomPartType()
{
	int randomIndex =  RndGen.Uni(0, PART_TYPES.size());
	return PART_TYPES[randomIndex];
}

vector<Node *> fS_Genotype::getAllNodes()
{
	vector<Node*> allNodes;
	startNode->getAllNodes(allNodes);
	return allNodes;
}

vector<Neuron *> fS_Genotype::getAllNeurons()
{
	vector<Neuron*> allNeurons;
	vector<Node*> allNodes = getAllNodes();
	for (unsigned int i = 0; i < allNodes.size(); i++)
	{
		for (unsigned int j = 0; j < allNodes[i]->neurons.size(); j++)
		{
			allNeurons.push_back(allNodes[i]->neurons[j]);
		}
	}
	return allNeurons;
}

Node *fS_Genotype::chooseNode(int fromIndex = 0)
{
	vector<Node*> allNodes = getAllNodes();
	return allNodes[RndGen.Uni(fromIndex, allNodes.size())];
}

int fS_Genotype::getNodeCount()
{
	return startNode->getNodeCount();
}

bool fS_Genotype::addJoint()
{
	if (startNode->childSize < 1)
		return false;

	Node *randomNode;    // First part does not have joints
	for (int i = 0; i < mutationTries; i++)
	{
		char randomJoint = JOINTS[RndGen.Uni(0, JOINT_COUNT)];
		randomNode = chooseNode(1);
		if (randomNode->joints.count(randomJoint) == 0)
		{
			randomNode->joints.insert(randomJoint);
			return true;
		}
	}
	return false;
}


bool fS_Genotype::removeJoint()
{
	// This operator may can lower success rate that others, as it does not work when there is only one node
	if (startNode->childSize < 1) // Only one node; there are no joints
		return false;

	// Choose a node with joints
	for (int i = 0; i < mutationTries; i++)
	{
		Node *randomNode = chooseNode(1);    // First part does not have joints
		int jointsCount = randomNode->joints.size();
		if (jointsCount >= 1)
		{
			int index = *(randomNode->joints.begin()) + RndGen.Uni(0, jointsCount);
			randomNode->joints.erase(index);
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
			advance(it, RndGen.Uni(0, paramCount));
			randomNode->params.erase(it->first);
			return true;
		}
	}
	return false;
}

bool fS_Genotype::changeParam()
{
	for (int i = 0; i < mutationTries; i++)
	{
		Node *randomNode = chooseNode();
		int paramCount = randomNode->params.size();
		if (paramCount >= 1)
		{
			auto it = randomNode->params.begin();
			advance(it, RndGen.Uni(0, paramCount));
			// TODO change parameters by more sensible values

			it->second += RndGen.Gauss(0, 0.5);
			if (it->second < 0)
				it->second *= -1;
			return true;
		}
	}
	return false;
}

bool fS_Genotype::addParam()
{
	Node *randomNode = chooseNode();
	unsigned int paramCount = randomNode->params.size();
	if (paramCount == PARAMS.size())
		return false;
	string chosenParam = PARAMS[RndGen.Uni(0, PARAMS.size())];
	// Not allow 'j' parameter when the cycle mode is not on
	if (chosenParam == JOINT_DISTANCE && !startNode->cycleMode)
		return false;
	if (randomNode->params.count(chosenParam) > 0)
		return false;
	// Add modified default value for param
	randomNode->params[chosenParam] = defaultParamValues.at(chosenParam);
	return true;
}

bool fS_Genotype::removePart()
{
	Node *randomNode, *chosenChild;
	// Choose a parent with children
	for (int i = 0; i < mutationTries; i++)
	{
		randomNode = chooseNode();
		int childCount = randomNode->childSize;
		if (childCount > 0)
		{
			chosenChild = randomNode->children[RndGen.Uni(0, childCount)];
			if (chosenChild->childSize == 0)
			{
				// Remove the chosen child
				swap(chosenChild, randomNode->children.back());
				randomNode->children.pop_back();
				randomNode->children.shrink_to_fit();
				delete chosenChild;
				randomNode->childSize -= 1;
				return true;
			}
		}
	}
	return false;
}

bool fS_Genotype::addPart()
{
	Node *randomNode = chooseNode();
	char partType= getRandomPartType();
	Substring substring(&partType, 0, 1);
	Node *newNode = new Node(substring, randomNode->modifierMode, randomNode->paramMode, randomNode->cycleMode);
	// Add random rotation
	newNode->params[ROT_X] = RndGen.Uni(90, -90);
	newNode->params[ROT_Y] = RndGen.Uni(90, -90);
	newNode->params[ROT_Z] = RndGen.Uni(90, -90);

	randomNode->children.push_back(newNode);
	randomNode->childSize++;
	return true;
}

bool fS_Genotype::changePartType()
{
	Node *randomNode = chooseNode();
	char newType = randomNode->partType;
	while (newType == randomNode->partType)
		newType = getRandomPartType();

	randomNode->partType = newType;
	return true;
}

bool fS_Genotype::addModifier()
{
	Node *randomNode = chooseNode();
	char randomModifier = MODIFIERS[RndGen.Uni(0,MODIFIERS.length())];
	if (rndUint(2) == 1)
		randomModifier = toupper(randomModifier);
	randomNode->modifiers.push_back(randomModifier);
	return true;
}

bool fS_Genotype::removeModifier()
{
	for (int i = 0; i < mutationTries; i++)
	{
		Node *randomNode = chooseNode();
		if (!(randomNode->modifiers.empty()))
		{
			randomNode->modifiers.pop_back();
			return true;
		}
	}
	return false;
}

void fS_Genotype::rearrangeNeuronConnections(Neuron *changedNeuron, int shift=1)
{
	if(shift != 1 && shift != -1)
		throw "Wrong value of shift";

	vector<Neuron*> neurons = getAllNeurons();
	int changedNeuronIndex = -1;
	for(unsigned int i=0; i<neurons.size(); i++)
	{
		if(changedNeuron == neurons[i])
		{
			changedNeuronIndex = i;
			break;
		}
	}
	if(changedNeuronIndex == -1)
		throw "Neuron not in all neurons";

	for(unsigned int i=0; i<neurons.size(); i++)
	{
		Neuron *n = neurons[i];
		std::map<int, double> newInputs;
		for (auto it = n->inputs.begin(); it != n->inputs.end(); ++it)
		{
			if(it->first < changedNeuronIndex)
				newInputs[it->first] = it->second;
			else if(it->first > changedNeuronIndex)
				newInputs[it->first + shift] = it->second;
			else if(it->first == changedNeuronIndex){
				if(shift == 1)
					newInputs[it->first + shift] = it->second;
				// If shift == -1, just delete the input
			}
		}
		n->inputs = newInputs;
	}
}

bool fS_Genotype::addNeuro()
{
	Node *randomNode = chooseNode();
	Neuron *newNeuron;
	NeuroClass *rndclass= GenoOperators::getRandomNeuroClass();
	if(rndclass == NULL)
		newNeuron = new Neuron();
	else
	{
		const char *name = rndclass->getName().c_str();
		newNeuron = new Neuron(name, sizeof(name) / sizeof(char));
		if(rndclass->prefinputs > 0)
		{
			// Create as many connections for the neuron as possible (at most prefinputs)
			vector<Neuron*> allNeurons = getAllNeurons();
			int connectionsToCreate = std::max((int)allNeurons.size(), rndclass->prefinputs);
			for(int i=0; i<connectionsToCreate; i++)
				newNeuron->inputs[i] = DEFAULT_NEURO_CONNECTION_WEIGHT;
		}
	}

	randomNode->neurons.push_back(newNeuron);

	rearrangeNeuronConnections(newNeuron);
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
			// Remove the chosen neuron
			int size = randomNode->neurons.size();
			Neuron *it = randomNode->neurons[RndGen.Uni(0,size)];
			rearrangeNeuronConnections(it, -1);		// Important to rearrange the neurons before deleting
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
	vector<Neuron*> neurons = getAllNeurons();
	if (neurons.empty())
		return false;

	int size = neurons.size();
	for (int i = 0; i < mutationTries; i++)
	{
		Neuron *chosenNeuron = neurons[RndGen.Uni(0,size)];
		if (!chosenNeuron->inputs.empty())
		{
			int inputCount = chosenNeuron->inputs.size();
			auto it = chosenNeuron->inputs.begin();
			advance(it, RndGen.Uni(0,inputCount));

			Neuro *neuro = new Neuro();
			SString details;
			details += chosenNeuron->cls;
			neuro->setDetails(details);
			it->second *= GenoOperators::mutateNeuProperty(it->second, neuro, -1);
			delete neuro;
			return true;
		}
	}
	return false;
}

bool fS_Genotype::addNeuroConnection()
{
	vector<Neuron*> neurons = getAllNeurons();
	if (neurons.empty())
		return false;

	int size = neurons.size();
	Neuron *chosenNeuron = neurons[RndGen.Uni(0,size)];

	for (int i = 0; i < mutationTries; i++)
	{
		int index = RndGen.Uni(0,size);
		if (chosenNeuron->inputs.count(index) == 0)
		{
			chosenNeuron->inputs[index] = DEFAULT_NEURO_CONNECTION_WEIGHT;
			return true;
		}
	}
	return false;
}

bool fS_Genotype::removeNeuroConnection()
{
	vector<Neuron*> neurons = getAllNeurons();
	if (neurons.empty())
		return false;

	int size = neurons.size();
	for (int i = 0; i < mutationTries; i++)
	{
		Neuron *chosenNeuron = neurons[RndGen.Uni(0,size)];
		if (!chosenNeuron->inputs.empty())
		{
			int inputCount = chosenNeuron->inputs.size();
			auto it = chosenNeuron->inputs.begin();
			advance(it, RndGen.Uni(0,inputCount));
			chosenNeuron->inputs.erase(it->first);
			return true;
		}
	}
	return false;
}

bool fS_Genotype::changeNeuroParam()
{
	// TODO implement
	return false;
}