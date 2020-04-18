//
// Created by jakub on 21.02.2020.
//

#include <iostream>
#include <stdlib.h>
#include<bits/stdc++.h>
#include <regex>
#include <time.h>
#include "fS_general.h"
#include <frams/model/geometry/geometryutils.h>

using namespace std;

#define MODIFIER_MODE 'M'
#define PARAM_MODE 'S'
#define CYCLE_MODE 'J'
#define BRANCH_START '('
#define BRANCH_END ')'
#define BRANCH_SEPARATOR ','
#define PARAM_START '{'
#define PARAM_END '}'
#define PARAM_SEPARATOR ';'
#define PARAM_KEY_VALUE_SEPARATOR '='

#define DEFAULT_JOINT 'a'
#define MULTIPLIER 1.1
#define JOINT_COUNT 4
#define SPHERE_RELATIVE_DISTANCE 0.25
#define MAX_DIAMETER_QUOTIENT 30

#define INGESTION "i"
#define FRICTION "f"
#define SIZE_X "x"
#define SIZE_Y "y"
#define SIZE_Z "z"
#define ROT_X "tx"
#define ROT_Y "ty"
#define ROT_Z "tz"
#define RX "rx"
#define RY "ry"
#define RZ "rz"
#define JOINT_DISTANCE "j"

#define DISJOINT 0
#define COLLISION 1
#define ADJACENT 2

const int mutationTries = 100;
const string PART_TYPES = "EPC";
const string JOINTS = "abcd";
const string OTHER_JOINTS = "bcd";
const string MODIFIERS = "ifxyz";
const vector <string> PARAMS{INGESTION, FRICTION, ROT_X, ROT_Y, ROT_Z, RX, RY, RZ, SIZE_X, SIZE_Y, SIZE_Z,
                             JOINT_DISTANCE};
const map<string, double> defaultParamValues = {
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
default_random_engine generator;
normal_distribution<double> distribution(0.0, 0.5);

double round2(double var) {
    double value = (int) (var * 100 + .5);
    return (double) value / 100;
}

const double operations[FS_OPCOUNT] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};

vector <SString> split(SString str, char delim) {
    // TODO optimize
    vector <SString> arr;
    int index = 0, new_index = 0, arrayIndex = 0;
    while (true) {
        new_index = str.indexOf(delim, index);
        if (new_index == -1) {
            arr.push_back(str.substr(index, INT_MAX));
            break;
        }
        arr.push_back(str.substr(index, new_index - index));
        arrayIndex += 1;
        index = new_index + 1;
    }
    return arr;
}

State::State(State *_state) {
    location = Pt3D(_state->location);
    v = Pt3D(_state->v);
    fr = _state->fr;
    sx = _state->sx;
    sy = _state->sy;
    sz = _state->sz;
}

State::State(Pt3D _location, Pt3D _v) {
    location = Pt3D(_location);
    v = Pt3D(_v);
}

void State::addVector(double length) {
    location += v * length;
}

void rotateVector(Pt3D &vector, Pt3D rotation) {
    // TOTO maybe optimize
    Orient rotmatrix = Orient_1;
    rotmatrix.rotate(Pt3D(
            Convert::toRadians(rotation.x),
            Convert::toRadians(rotation.y),
            Convert::toRadians(rotation.z)
    ));
    vector = rotmatrix.transform(vector);
}

void State::rotate(Pt3D rotation) {
    rotateVector(v, rotation);
    v.normalize();
}

Node::Node(const SString &genotype, bool _modifierMode, bool _paramMode, bool _cycleMode, bool _isStart = false) {
    isStart = _isStart;
    modifierMode = _modifierMode;
    paramMode = _paramMode;
    cycleMode = _cycleMode;
    SString restOfGenotype = extractModifiers(genotype);
    restOfGenotype = extractPartType(restOfGenotype);
    if (restOfGenotype.len() > 0 && restOfGenotype[0] == PARAM_START)
        restOfGenotype = extractParams(restOfGenotype);

    if (restOfGenotype.len() > 0)
        getChildren(restOfGenotype);
}

Node::~Node() {
    if (state != nullptr)
        delete state;
    for (unsigned int i = 0; i < childSize; i++)
        delete children[i];
}

int Node::getPartPosition(SString restOfGenotype) {
    for (int i = 0; i < restOfGenotype.len(); i++) {
        char tmp = restOfGenotype[i];
        if (tmp == 'E' || tmp == 'P' || tmp == 'C')
            return i;
    }
    return -1;
}

SString Node::extractModifiers(SString restOfGenotype) {
    int partTypePosition = getPartPosition(restOfGenotype);
    if (partTypePosition == -1)
        throw "Part type missing";
    // Get a string containing all modifiers and joints for this node
    SString modifierString = restOfGenotype.substr(0, partTypePosition);

    for (int i = 0; i < modifierString.len(); i++) {
        // Extract modifiers and joints
        char mType = modifierString[i];
        if (OTHER_JOINTS.find(mType) != string::npos)
            joints.insert(mType);
        else if (MODIFIERS.find(tolower(mType)) != string::npos)
            modifiers.push_back(mType);
        else
            throw "Invalid modifier";
    }
    return restOfGenotype.substr(partTypePosition, INT_MAX);
}

SString Node::extractPartType(SString restOfGenotype) {
    partType = restOfGenotype[0];
    if (PART_TYPES.find(partType) == string::npos)
        throw "Invalid part type";
    return restOfGenotype.substr(1, INT_MAX);
}

SString Node::extractParams(SString restOfGenotype) {
    int paramsEndIndex = restOfGenotype.indexOf(PARAM_END);
    SString paramString = restOfGenotype.substr(1, paramsEndIndex - 1);
    vector <SString> keyValuePairs = split(paramString, PARAM_SEPARATOR);
    for (unsigned int i = 0; i < keyValuePairs.size(); i++) {
        SString keyValue = keyValuePairs[i];
        int separatorIndex = keyValuePairs[i].indexOf(PARAM_KEY_VALUE_SEPARATOR);
        if (-1 == separatorIndex)
            throw "Parameter separator expected";
        string key = keyValue.substr(0, separatorIndex).c_str();
        // TODO optimize, handle wrong value
        double value = atof(keyValue.substr(separatorIndex + 1, INT_MAX).c_str());
        params[key] = value;
    }

    return restOfGenotype.substr(paramsEndIndex + 1, INT_MAX);
}

double Node::getParam(string key) {
    auto item = params.find(key);
    if (item != params.end())
        return item->second;
    else
        return defaultParamValues.at(key);
}

/// Get distance

double avg(double a, double b) {
    return 0.5 * (a + b);
}

double min3(Pt3D p) {
    double tmp = p.x;
    if (p.y < tmp)
        tmp = p.y;
    if (p.z < tmp)
        tmp = p.z;
    return tmp;
}

double max3(Pt3D p) {
    double tmp = p.x;
    if (p.y > tmp)
        tmp = p.y;
    if (p.z > tmp)
        tmp = p.z;
    return tmp;
}

double getSphereCoordinate(double dimension, double sphereDiameter, double index, int count) {
    if (count == 1)
        return 0;
    return (dimension - sphereDiameter) * (index / (count - 1) - 0.5);
}

Pt3D *findSphereCenters(int &sphereCount, double &sphereRadius, Pt3D radii, Pt3D rotations) {
    double sphereRelativeDistance = SPHERE_RELATIVE_DISTANCE;
    double maxDiameterQuotient = MAX_DIAMETER_QUOTIENT;
    double minRadius = min3(radii);
    double maxRadius = max3(radii);
    if (maxRadius / minRadius < maxDiameterQuotient) // WHen max radius is much bigger than min radius
        sphereRadius = minRadius;
    else {
        sphereRelativeDistance = 1.0;   // Make the spheres adjacent to speed up the computation
        sphereRadius = maxRadius / maxDiameterQuotient;
    }
    double sphereDiameter = 2 * sphereRadius;

    double *diameters = new double[3]{2 * radii.x, 2 * radii.y, 2 * radii.z};
    int counts[3];
    for (int i = 0; i < 3; i++) {
        counts[i] = 1;
        if (diameters[i] > sphereDiameter)
            counts[i] += ceil((diameters[i] - sphereDiameter) / sphereDiameter / sphereRelativeDistance);
    }

    sphereCount = counts[0] * counts[1] * counts[2];
    double x, y, z;
    int totalCount = 0;
    Pt3D *centers = new Pt3D[sphereCount];
    for (double xi = 0; xi < counts[0]; xi++) {
        x = getSphereCoordinate(diameters[0], sphereDiameter, xi, counts[0]);
        for (double yi = 0; yi < counts[1]; yi++) {
            y = getSphereCoordinate(diameters[1], sphereDiameter, yi, counts[1]);
            for (double zi = 0; zi < counts[2]; zi++) {
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

int isCollision(Pt3D *centersParent, Pt3D *centers, int parentSphereCount, int sphereCount, Pt3D vector,
                double distanceThreshold) {
//    return ADJACENT;
    double toleration = 0.999;
//    double upperThreshold = 1.0001 * distanceThreshold;
//    double lowerThreshold = 0.9999 * toleration * distanceThreshold;
    double upperThreshold = distanceThreshold;
    double lowerThreshold = toleration * distanceThreshold;
    double distance;
    double dx, dy, dz;
    bool existsAdjacent = false;
    Pt3D *tmpPoint;
    for (int sc = 0; sc < sphereCount; sc++) {
        Pt3D shiftedSphere = Pt3D(centers[sc]);
        shiftedSphere += vector;
        for (int psc = 0; psc < parentSphereCount; psc++) {
            tmpPoint = &centersParent[psc];
            dx = shiftedSphere.x - tmpPoint->x;
            dy = shiftedSphere.y - tmpPoint->y;
            dz = shiftedSphere.z - tmpPoint->z;
            distance = sqrt(dx * dx + dy * dy + dz * dz);

            if (distance <= upperThreshold) {
                if (distance >= lowerThreshold)
                    existsAdjacent = true;
                else {
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

double getDistance(Pt3D radiiParent, Pt3D radii, Pt3D vector, Pt3D rotationParent, Pt3D rotation) {
    int parentSphereCount, sphereCount;
    double parentSphereRadius, sphereRadius;
    Pt3D *centersParent = findSphereCenters(parentSphereCount, parentSphereRadius, radiiParent, rotationParent);
    Pt3D *centers = findSphereCenters(sphereCount, sphereRadius, radii, rotation);

    double distanceThreshold = sphereRadius + parentSphereRadius;
    double minDistance = 0.0;   // TO make sure that program will not process forever
    double maxDistance = 2 * (max3(radiiParent) + max3(radii));
    double currentDistance = avg(maxDistance, minDistance);
    int result = -1;
    while (result != ADJACENT) {
        Pt3D currentVector = vector * currentDistance;
        result = isCollision(centersParent, centers, parentSphereCount, sphereCount, currentVector, distanceThreshold);
        if (result == DISJOINT) {
            maxDistance = currentDistance;
            currentDistance = avg(currentDistance, minDistance);
        } else if (result == COLLISION) {
            minDistance = currentDistance;
            currentDistance = avg(maxDistance, currentDistance);
        }
        // TODO decide what to do
        if (currentDistance > maxDistance) {
            cout << "Warning" << endl;
            currentDistance = maxDistance;
            break;
        }
        if (currentDistance < minDistance) {
            cout << "Warning" << endl;
            currentDistance = minDistance;
            break;
        }

    }

    delete[] centersParent;
    delete[] centers;
    return round2(currentDistance);
}
/// Get distance

void Node::getState(State *_state, Pt3D parentSize) {
    if (isStart)
        state = _state;
    else
        state = new State(_state);


    // Update state by modifiers
    for (unsigned int i = 0; i < modifiers.size(); i++) {
        char mod = modifiers[i];
        double multiplier = isupper(mod) ? MULTIPLIER : 1.0 / MULTIPLIER;
        switch (tolower(mod)) {
            case 'i':
                state->ing *= multiplier;
                break;
            case 'f':
                state->fr *= multiplier;
                break;
            case 'x':
                state->sx *= multiplier;
                break;
            case 'y':
                state->sy *= multiplier;
                break;
            case 'z':
                state->sz *= multiplier;
                break;
        }
    }

    if (!isStart) {
        // Rotate
        Pt3D size = getSize();
        state->rotate(getVectorRotation());

        double distance = getDistance(parentSize, size, state->v, getRotation(), getRotation());
        state->addVector(distance);
    }
}

void Node::getChildren(SString restOfGenotype) {
    vector <SString> branches = getBranches(restOfGenotype);
    childSize = branches.size();
    for (unsigned int i = 0; i < childSize; i++)
        children.push_back(new Node(branches[i], modifierMode, paramMode, cycleMode));
}

vector <SString> Node::getBranches(SString restOfGenotype) {
    if (restOfGenotype[0] != BRANCH_START) {
        vector <SString> result{restOfGenotype};  // Only one child
        return result;
    }
    // TODO handle wrong syntax

    int depth = 0;
    int start = 1;
    vector <SString> children;
    int length = restOfGenotype.len();
    for (int i = 0; i < restOfGenotype.len(); i++) {
        char c = restOfGenotype[i];
        if (c == BRANCH_START)
            depth += 1;
        else if ((c == BRANCH_SEPARATOR && depth == 1) || i + 1 == length) {
            children.push_back(restOfGenotype.substr(start, i - start));
            start = i + 1;
        } else if (c == BRANCH_END)
            depth -= 1;
    }
    return children;
}

Pt3D Node::getSize() {
    double sx = getParam(SIZE_X) * state->sx;
    double sy = getParam(SIZE_Y) * state->sy;
    double sz = getParam(SIZE_Z) * state->sz;
    return Pt3D(sx, sy, sz);
}

Pt3D Node::getVectorRotation() {
    double rx = getParam(ROT_X);
    double ry = getParam(ROT_Y);
    double rz = getParam(ROT_Z);
    return Pt3D(rx, ry, rz);
}

Pt3D Node::getRotation() {
    double rx = getParam(RX);
    double ry = getParam(RY);
    double rz = getParam(RZ);
    return Pt3D(rx, ry, rz);
}

Part *Node::buildModel(Model *model) {
    createPart();
    model->addPart(part);

    for (unsigned int i = 0; i < childSize; i++) {
        Node *child = children[i];
        child->getState(state, getSize());
        child->buildModel(model);
        addJointsToModel(model, child);
    }
    return part;
}

void Node::createPart() {
    Part::Shape model_partType;
    if (partType == 'E')
        model_partType = Part::Shape::SHAPE_ELLIPSOID;
    else if (partType == 'P')
        model_partType = Part::Shape::SHAPE_CUBOID;
    else if (partType == 'C')
        model_partType = Part::Shape::SHAPE_CYLINDER;
    part = new Part(model_partType);

    part->p = Pt3D(round2(state->location.x),
                   round2(state->location.y),
                   round2(state->location.z)
    );
    part->friction = round2(getParam(FRICTION) * state->fr);
    part->ingest = round2(getParam(INGESTION) * state->ing);
    Pt3D size = getSize();
    part->scale.x = round2(size.x);
    part->scale.y = round2(size.y);
    part->scale.z = round2(size.z);
    part->setRot(getRotation());
}

void Node::addJointsToModel(Model *model, Node *child) {
    if (child->joints.empty()) {
        Joint *joint = new Joint();
        joint->shape = Joint::Shape::SHAPE_FIXED;
        joint->attachToParts(part, child->part);
        model->addJoint(joint);
    } else {
        for (auto it = child->joints.begin(); it != child->joints.end(); ++it) {
            Joint *joint = new Joint();
            joint->attachToParts(part, child->part);
            switch (*it) {
                case 'b':
                    joint->shape = Joint::Shape::SHAPE_B;
                    break;
                case 'c':
                    joint->shape = Joint::Shape::SHAPE_C;
                    break;
                case 'd':
                    joint->shape = Joint::Shape::SHAPE_D;
                    break;
            }
            model->addJoint(joint);
        }
    }
}


void Node::getGeno(SString &result) {
    for (auto it = joints.begin(); it != joints.end(); ++it)
        result += *it;
    for (auto it = modifiers.begin(); it != modifiers.end(); ++it)
        result += *it;
    result += partType;

    if (params.size() > 0) {
        result += PARAM_START;
        for (auto it = params.begin(); it != params.end(); ++it) {
            result += it->first.c_str();
            result += PARAM_KEY_VALUE_SEPARATOR;
            string value_text = to_string(it->second);
            result += value_text.substr(0, value_text.find(".") + 2).c_str(); // ROund
            result += PARAM_SEPARATOR;
        }
        result = result.substr(0, result.len() - 1);
        result += PARAM_END;
    }

    if (childSize == 1)
        children[0]->getGeno(result);
    else if (childSize > 1) {
        result += BRANCH_START;
        for (unsigned int i = 0; i < childSize - 1; i++) {
            children[i]->getGeno(result);
            result += BRANCH_SEPARATOR;
        }
        children[childSize - 1]->getGeno(result);
        result += BRANCH_END;
    }
}

void Node::getAllNodes(vector<Node *> &allNodes) {
    allNodes.push_back(this);
    for (unsigned int i = 0; i < childSize; i++)
        children[i]->getAllNodes(allNodes);
}

fS_Genotype::fS_Genotype(const SString &genotype) {
    // M - modifier mode, S - standard mode
    int modeSeparatorIndex = genotype.indexOf(':');
    if (modeSeparatorIndex == -1)
        throw "No mode separator";

    SString modeStr = genotype.substr(0, modeSeparatorIndex);
    bool modifierMode = -1 != modeStr.indexOf(MODIFIER_MODE);
    bool paramMode = -1 != modeStr.indexOf(PARAM_MODE);
    bool cycleMode = -1 != modeStr.indexOf(CYCLE_MODE);
    startNode = new Node(genotype.substr(modeSeparatorIndex + 1, INT_MAX), modifierMode, paramMode, cycleMode, true);
}

fS_Genotype::~fS_Genotype() {
    delete startNode;
}

void fS_Genotype::buildModel(Model *model) {
    State *initialState = new State(Pt3D(0), Pt3D(1, 0, 0));
    startNode->getState(initialState, Pt3D(1.0));
    startNode->buildModel(model);

    // Additional joints
    vector < Node * > allNodes = getAllNodes();
    for (unsigned int i = 0; i < allNodes.size(); i++) {
        Node *node = allNodes[i];
        if (node->params.find(JOINT_DISTANCE) != node->params.end()) {
            Node *otherNode = getNearestNode(allNodes, node);
            if (otherNode != nullptr) {
                // If other node is close enough, add a joint
                double distance = node->state->location.distanceTo(otherNode->state->location);
                if (distance < node->params[JOINT_DISTANCE]) {
                    Joint *joint = new Joint();
                    joint->attachToParts(node->part, otherNode->part);

                    joint->shape = Joint::Shape::SHAPE_FIXED;
                    model->addJoint(joint);
                }
            }
        }
    }
}

Node *fS_Genotype::getNearestNode(vector<Node *> allNodes, Node *node) {
    Node *result = nullptr;
    double minDistance = 9999999.0, distance = 999999.0;
    for (unsigned int i = 0; i < allNodes.size(); i++) {
        Node *otherNode = allNodes[i];
        auto v = node->children;
        if (otherNode != node &&
            find(v.begin(), v.end(), otherNode) == v.end()) {   // Not the same node and not a child
            distance = node->state->location.distanceTo(otherNode->state->location);
            if (distance < minDistance) {
                minDistance = distance;
                result = otherNode;
            }
        }
    }
    return result;
}

SString fS_Genotype::getGeno() {
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

int fS_Genotype::getNodeCount() {
    vector < Node * > allNodes = getAllNodes();
    return allNodes.size();
}

int fS_Genotype::randomFromRange(int to, int from = 0) {
    return from + rand() % (to - from);
}

double getRandomFromDistribution() {
    return distribution(generator);
}

vector<Node *> fS_Genotype::getAllNodes() {
    vector < Node * > allNodes;
    startNode->getAllNodes(allNodes);
    return allNodes;
}

Node *fS_Genotype::chooseNode(int fromIndex = 0) {
    vector < Node * > allNodes = getAllNodes();
    return allNodes[randomFromRange(allNodes.size(), fromIndex)];
}


bool fS_Genotype::addJoint() {
    if (startNode->childSize < 1)
        return false;

    Node *randomNode;    // First part does not have joints
    bool success = false;
    for(int i=0; i<mutationTries; i++) {
        char randomJoint = JOINTS[randomFromRange(JOINT_COUNT, 1)];
        randomNode = chooseNode(1);
        if (randomNode->joints.count(randomJoint) == 0) {
            randomNode->joints.insert(randomJoint);
            success = true;
            break;
        }
    }

    return success;
}


bool fS_Genotype::removeJoint() {
    // This operator may can lower success rate that others, as it does not work when there is only one node
    if (startNode->childSize < 1) // Only one node; there are no joints
        return false;

    // Choose a node with joints
    Node *randomNode;
    int jointsCount = 0, tries = 0;
    while(tries < mutationTries && jointsCount < 1) {
        randomNode = chooseNode(1);    // First part does not have joints
        jointsCount = randomNode->joints.size();
        tries += 1;
    }
    if (jointsCount < 1)
        return false;

    int index = *(randomNode->joints.begin()) + randomFromRange(jointsCount);
    randomNode->joints.erase(index);

    return true;
}


bool fS_Genotype::removeParam() {
    // Choose a node with params
    Node *randomNode;
    int paramCount = 0, tries = 0;
    while (tries < mutationTries && paramCount < 1) {
        randomNode = chooseNode();
        paramCount = randomNode->params.size();
        tries += 1;
    }
    if (paramCount < 1)
        return false;

    auto it = randomNode->params.begin();
    advance(it, randomFromRange(paramCount));
    randomNode->params.erase(it->first);
    return true;
}

bool fS_Genotype::changeParam() {
    Node *randomNode;
    int paramCount = 0, tries = 0;
    while (tries < mutationTries && paramCount < 1) {
        randomNode = chooseNode();
        paramCount = randomNode->params.size();
        tries += 1;
    }
    if (paramCount < 1)
        return false;

    auto it = randomNode->params.begin();
    advance(it, randomFromRange(paramCount));
    // TODO sensible parameter changes

    it->second += getRandomFromDistribution();
    if (it->second < 0)
        it->second *= -1;
    return true;
}

bool fS_Genotype::addParam() {
    Node *randomNode = chooseNode();
    unsigned int paramCount = randomNode->params.size();
    if (paramCount == PARAMS.size())
        return false;
    string chosenParam = PARAMS[randomFromRange(PARAMS.size())];
    // Not allow jd when cycle mode is not on
    if (chosenParam == JOINT_DISTANCE && !startNode->cycleMode)
        return false;
    if (randomNode->params.count(chosenParam) > 0)
        return false;
    // Add modified default value for param
    randomNode->params[chosenParam] = defaultParamValues.at(chosenParam);
    return true;
}

bool fS_Genotype::removePart() {
    Node *randomNode, *chosenChild;
    bool success = false;
    // Choose a parent with children
    for(int i=0; i<mutationTries; i++) {
        randomNode = chooseNode();
        int childCount = randomNode->childSize;
        if(childCount > 0){
            chosenChild = randomNode->children[randomFromRange(childCount)];
            if(chosenChild->childSize == 0) {
                success = true;
                break;
            }
        }
    }
    if (!success)
        return false;

    // Remove the chosen child
    swap(chosenChild, randomNode->children.back());
    randomNode->children.pop_back();
    randomNode->children.shrink_to_fit();
    delete chosenChild;
    randomNode->childSize -= 1;
    return true;
}

bool fS_Genotype::addPart() {
    Node *randomNode = chooseNode();
    SString partType;
    partType += PART_TYPES[randomFromRange(3, 0)];
    Node *newNode = new Node(partType, randomNode->modifierMode, randomNode->paramMode, randomNode->cycleMode);
    // Add random rotation
    newNode->params["tx"] = randomFromRange(90, -90);
    newNode->params["ty"] = randomFromRange(90, -90);
    newNode->params["tz"] = randomFromRange(90, -90);

    randomNode->children.push_back(newNode);
    randomNode->childSize += 1;
    return true;
}

bool fS_Genotype::changePartType() {
    Node *randomNode = chooseNode();
    char newType = randomNode->partType;
    while (newType == randomNode->partType)
        newType = PART_TYPES[randomFromRange(PART_TYPES.size())];

    randomNode->partType = newType;
    return true;
}

bool fS_Genotype::addModifier() {
    Node *randomNode = chooseNode();
    char randomModifier = MODIFIERS[randomFromRange(MODIFIERS.length())];
    if (1 == random() % 2)
        randomModifier = toupper(randomModifier);
    randomNode->modifiers.push_back(randomModifier);
    return true;
}

bool fS_Genotype::removeModifier() {
    Node *randomNode = chooseNode();
    int tries = 0;
    while (tries < mutationTries && randomNode->modifiers.empty()) {
        randomNode = chooseNode();
        tries += 1;
    }
    if (randomNode->modifiers.empty())
        return false;

    randomNode->modifiers.pop_back();
    return true;
}

void fS_Genotype::mutate(int &method) {
//    int operationCount = 5;
    double operations[FS_OPCOUNT] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,};
    if (!startNode->paramMode) {
        operations[5] = 0.0;
        operations[6] = 0.0;
        operations[7] = 0.0;
    }
    if (!startNode->modifierMode) {
        operations[8] = 0.0;
        operations[9] = 0.0;
    }

    bool result = false;
    while (!result) {
        method = GenoOperators::roulette(operations, FS_OPCOUNT);
        switch (method) {
            case 0:
                result = addPart();
                break;
            case 1:
                result = removePart();
                break;
            case 2:
                result = changePartType();
                break;
            case 3:
                result = addJoint();
                break;
            case 4:
                result = removeJoint();
                break;
            case 5:
                result = addParam();
                break;
            case 6:
                result = removeParam();
                break;
            case 7:
                result = changeParam();
                break;
            case 8:
                result = addModifier();
                break;
            case 9:
                result = removeModifier();
                break;
        }
    }
}

