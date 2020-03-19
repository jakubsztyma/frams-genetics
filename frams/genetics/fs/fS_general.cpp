//
// Created by jakub on 21.02.2020.
//

#include <iostream>
#include <stdlib.h>
#include<bits/stdc++.h>
#include <regex>
#include <time.h>
#include "fS_general.h"

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

const string PART_TYPES = "EPC";
const string JOINTS = "abcd";
const string OTHER_JOINTS = "bcd";
const string MODIFIERS = "fx";
const vector <string> PARAMS{"fr", "rx", "ry", "rz", "sx", "sy", "sz",  "jd"};
const map<string, double> defaultParamValues = {
        {"fr", 0.4},
        {"rx", 0.0},
        {"ry", 0.0},
        {"rz", 0.0},
        {"sx", 1.0},
        {"sy", 1.0},
        {"sz", 1.0},
        {"jd", 1.0}
};
default_random_engine generator;
normal_distribution<double> distribution(0.0, 0.1);

double round2(double var) {
    double value = (int) (var * 100 + .5);
    return (double) value / 100;
}

const double operations[FS_OPCOUNT] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};

vector<SString> split(SString str, char delim) {
    // TODO optimize
    vector<SString> arr;
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

Mode::Mode(SString modeStr){
    modifier = -1 != modeStr.indexOf(MODIFIER_MODE);
    param = -1 != modeStr.indexOf(PARAM_MODE);
    cycle = -1 != modeStr.indexOf(CYCLE_MODE);
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

void State::rotate(double rx, double ry, double rz) {
    // TOTO maybe optimize
    Orient rotmatrix = Orient_1;
    rotmatrix.rotate(Pt3D(
            Convert::toRadians(rx),
            Convert::toRadians(ry),
            Convert::toRadians(rz)
    ));
    v = rotmatrix.transform(v);
    v.normalize();
}

Node::Node(const SString &genotype, Mode *_mode, bool _isStart = false) {
    isStart = _isStart;
    mode = _mode;
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
    // Get a string containing all modifiers and joints for this node
    SString modifierString = restOfGenotype.substr(0, partTypePosition);

    for (int i = 0; i < modifierString.len(); i++) {
        // Extract modifiers and joints
        char mType = modifierString[i];
        if (OTHER_JOINTS.find(mType) != string::npos)
            joints.insert(mType);
        else
            modifiers.push_back(mType);
    }
    return restOfGenotype.substr(partTypePosition, INT_MAX);
}

SString Node::extractPartType(SString restOfGenotype) {
    partType = restOfGenotype[0];
    return restOfGenotype.substr(1, INT_MAX);
}

SString Node::extractParams(SString restOfGenotype) {
    int paramsEndIndex = restOfGenotype.indexOf(PARAM_END);
    SString paramString = restOfGenotype.substr(1, paramsEndIndex - 1);
    vector<SString> keyValuePairs = split(paramString, PARAM_SEPARATOR);
    for (unsigned int i = 0; i < keyValuePairs.size(); i++) {
        SString keyValue = keyValuePairs[i];
        int separatorIndex = keyValuePairs[i].indexOf(PARAM_KEY_VALUE_SEPARATOR);
        // TODO handle wrong length exception
        // TODO optimize
        double value = atof(keyValue.substr(separatorIndex + 1, INT_MAX).c_str());
        params[keyValue.substr(0, separatorIndex).c_str()] = value;
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

void Node::getState(State *_state, double psx, double psy, double psz) {
    if (isStart)
        state = _state;
    else
        state = new State(_state);


    // Update state by modifiers
    for (unsigned int i = 0; i < modifiers.size(); i++) {
        char mod = modifiers[i];
        double multiplier = isupper(mod) ? MULTIPLIER : 1.0 / MULTIPLIER;
        switch (tolower(mod)) {
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

    if (!isStart){
        // Rotate
        double rx = getParam("rx");
        double ry = getParam("ry");
        double rz = getParam("rz");
        double sx = getSx();
        double sy = getSy();
        double sz = getSz();
        state->rotate(rx, ry, rz);

        double distance = (psx + psy + psz + sx + sy + sz) / 3;
        state->addVector(distance);
    }
}

void Node::getChildren(SString restOfGenotype) {
    vector <SString> branches = getBranches(restOfGenotype);
    childSize = branches.size();
    for (unsigned int i = 0; i < childSize; i++)
        children.push_back(new Node(branches[i], mode));
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

double Node::getSx(){return getParam("sx") * state->sx;}
double Node::getSy(){return getParam("sy") * state->sy;}
double Node::getSz(){return getParam("sz") * state->sz;}

Part *Node::buildModel(Model *model) {
    createPart();
    model->addPart(part);

    for (unsigned int i = 0; i < childSize; i++) {
        Node *child = children[i];
        child->getState(state, getSx(), getSy(), getSz());
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
    part->friction = round2(getParam("fr") * state->fr);
    part->scale.x = round2(getSx());
    part->scale.y = round2(getSy());
    part->scale.z = round2(getSz());
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
    SString modeStr = genotype.substr(0, modeSeparatorIndex);
    Mode *mode = new Mode(modeStr);
    startNode = new Node(genotype.substr(modeSeparatorIndex + 1, INT_MAX), mode, true);
}

fS_Genotype::~fS_Genotype() {
    delete startNode->mode;
    delete startNode;
}

void fS_Genotype::buildModel(Model *model) {
    State *initialState = new State(Pt3D(0), Pt3D(1, 0, 0));
    startNode->getState(initialState, 1.0, 1.0, 1.0);
    startNode->buildModel(model);

    // Additional joints
    vector < Node * > allNodes = getAllNodes();
    for (unsigned int i = 0; i < allNodes.size(); i++) {
        Node *node = allNodes[i];
        if (node->params.find("jd") != node->params.end()) {
            Node *otherNode = getNearestNode(allNodes, node);
            // If other node is close enough, add a joint
            if (node->state->location.distanceTo(otherNode->state->location) < node->params["jd"]) {
                Joint *joint = new Joint();
                joint->attachToParts(node->part, otherNode->part);

                joint->shape = Joint::Shape::SHAPE_FIXED;
                model->addJoint(joint);
            }
        }
    }
}

Node *fS_Genotype::getNearestNode(vector<Node *> allNodes, Node *node) {
    Node *result;
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

    if(startNode->mode->modifier)
        geno += MODIFIER_MODE;
    if(startNode->mode->param)
        geno += PARAM_MODE;
    if(startNode->mode->cycle)
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

    Node *randomNode = chooseNode(1);    // First part does not have joints
    char randomJoint = JOINTS[randomFromRange(JOINT_COUNT, 1)];
    if (randomNode->joints.count(randomJoint) > 0)
        return false;

    randomNode->joints.insert(randomJoint);
    return true;
}


bool fS_Genotype::removeJoint() {
    if (startNode->childSize < 1) // Only one node; there are no joints
        return false;

    Node *randomNode = chooseNode(1);    // First part does not have joints
    int jointsCount = randomNode->joints.size();
    if (jointsCount < 1)
        return false;
    int index = *(randomNode->joints.begin()) + randomFromRange(jointsCount);
    randomNode->joints.erase(index);

    return true;
}


bool fS_Genotype::removeParam() {
    Node *randomNode = chooseNode();
    int paramCount = randomNode->params.size();
    if (paramCount < 1)
        return false;
    auto it = randomNode->params.begin();
    advance(it, randomFromRange(paramCount));
    randomNode->params.erase(it->first);
    return true;
}

bool fS_Genotype::changeParam() {
    Node *randomNode = chooseNode();
    int paramCount = randomNode->params.size();
    if (paramCount < 1)
        return false;
    auto it = randomNode->params.begin();
    advance(it, randomFromRange(paramCount));
    // TODO sensible parameter changes
    it->second += getRandomFromDistribution();
    return true;
}

bool fS_Genotype::addParam() {
    Node *randomNode = chooseNode();
    unsigned int paramCount = randomNode->params.size();
    if (paramCount == PARAMS.size())
        return false;
    string chosenParam = PARAMS[randomFromRange(PARAMS.size())];
    // Not allow jd when cycle mode is not on
    if(chosenParam == "jd" && !startNode->mode->cycle)
        return false;
    if (randomNode->params.count(chosenParam) > 0)
        return false;
    // Add modified default value for param
    randomNode->params[chosenParam] = defaultParamValues.at(chosenParam) + getRandomFromDistribution();
    return true;
}

bool fS_Genotype::removePart() {
    Node *randomNode = chooseNode();
    int childCount = randomNode->childSize;
    if (childCount < 1)
        return false;
    Node *chosenNode = randomNode->children[randomFromRange(childCount)];
    if (chosenNode->childSize > 0)
        return false;

    swap(chosenNode, randomNode->children.back());
    randomNode->children.pop_back();
    randomNode->children.shrink_to_fit();
    delete chosenNode;
    randomNode->childSize -= 1;
    return true;
}

bool fS_Genotype::addPart() {
    Node *randomNode = chooseNode();
    SString partType = PART_TYPES[0];
    Node *newNode = new Node(partType, randomNode->mode);
    randomNode->children.push_back(newNode);
    randomNode->childSize += 1;
    return true;
}

bool fS_Genotype::changePartType() {
    Node *randomNode = chooseNode();
    char newType = PART_TYPES[randomFromRange(PART_TYPES.size())];
    if (newType == randomNode->partType)
        return false;
    randomNode->partType = newType;
    return true;
}

bool fS_Genotype::addModifier() {
    Node *randomNode = chooseNode();
    char randomModifier = MODIFIERS[randomFromRange(MODIFIERS.length())];
    if(1 == random() % 2)
        randomModifier = toupper(randomModifier);
    randomNode->modifiers.push_back(randomModifier);
    return true;
}

bool fS_Genotype::removeModifier() {
    Node *randomNode = chooseNode();
    if (randomNode->modifiers.empty())
        return false;
    randomNode->modifiers.pop_back();
    return true;
}

void fS_Genotype::mutate() {
//    int operationCount = 5;
    double operations[FS_OPCOUNT] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, };
    if(!startNode->mode->param){
        operations[5] = 0.0;
        operations[6] = 0.0;
        operations[7] = 0.0;
    }
    if(!startNode->mode->modifier) {
        operations[8] = 0.0;
        operations[9] = 0.0;
    }

    bool result = false;
    int method;
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

