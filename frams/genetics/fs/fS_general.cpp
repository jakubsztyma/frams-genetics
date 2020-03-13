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
#define STANDARD_MODE 'S'
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
#define DEFAULT_FR 0.4


const string PART_TYPES = "EPC";
const vector <string> PARAMS{"fr", "rx", "ry", "rz", "jd"};
const string JOINTS = "abcd";
const string OTHER_JOINTS = "bcd";
const string MODIFIERS = "Ff";
default_random_engine generator;
normal_distribution<double> distribution(0.0, 0.1);

double round2(double var) {
    double value = (int) (var * 100 + .5);
    return (double) value / 100;
}

const double operations[FS_OPCOUNT] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};

SString *split(SString str, char delim, int count) {
    // TODO optimize
    static SString *arr = new SString[count];
    int index = 0, new_index = 0, arrayIndex = 0;
    while (true) {
        new_index = str.indexOf(delim, index);
        if (new_index == -1) {
            arr[arrayIndex] = str.substr(index, INT_MAX);
            break;
        }
        arr[arrayIndex] = str.substr(index, new_index - index);
        arrayIndex += 1;
        index = new_index + 1;
    }
    return arr;
}

State::State(State *_state) {
    location = Pt3D(_state->location);
    v = Pt3D(_state->v);
    fr = _state->fr;
    modifierMode = _state->modifierMode;
}

State::State(Pt3D _location, Pt3D _v, bool _modifierMode) {
    location = Pt3D(_location);
    v = Pt3D(_v);
    fr = DEFAULT_FR;
    modifierMode = _modifierMode;
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

Node::Node(const SString &genotype, State *_state, bool _isStart = false) {
    isStart = _isStart;
    SString restOfGenotype = extractModifiers(genotype);
    restOfGenotype = extractPartType(restOfGenotype);
    if (restOfGenotype.len() > 0 && restOfGenotype[0] == PARAM_START)
        restOfGenotype = extractParams(restOfGenotype);

    getState(_state);
    if (restOfGenotype.len() > 0)
        getChildren(restOfGenotype);
}

Node::~Node() {
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
    part_type = restOfGenotype[0];
    return restOfGenotype.substr(1, INT_MAX);
}

SString Node::extractParams(SString restOfGenotype) {
    int paramsEndIndex = restOfGenotype.indexOf(PARAM_END);
    SString paramString = restOfGenotype.substr(1, paramsEndIndex - 1);
    unsigned int size = 1;
    SString *keyValuePairs = split(paramString, PARAM_SEPARATOR, size);
    for (unsigned int i = 0; i < size; i++) {
        SString keyValue = keyValuePairs[i];
        int separatorIndex = keyValuePairs[i].indexOf(PARAM_KEY_VALUE_SEPARATOR);
        // TODO handle wrong length exception
        // TODO optimize
        double value = atof(keyValue.substr(separatorIndex + 1, INT_MAX).c_str());
        params[keyValue.substr(0, separatorIndex).c_str()] = value;
    }

    return restOfGenotype.substr(paramsEndIndex + 1, INT_MAX);
}

void Node::getState(State *_state) {
    if (isStart) {
        state = _state;
    } else {
        state = new State(_state);

        // Rotate
        float rx = 0, ry = 0, rz = 0;
        auto paramsEnd = params.end();
        auto rxi = params.find("rx");
        auto ryi = params.find("ry");
        auto rzi = params.find("rz");
        if (rxi != paramsEnd)
            rx = rxi->second;
        if (ryi != paramsEnd)
            ry = ryi->second;
        if (rzi != paramsEnd)
            rz = rzi->second;
        state->rotate(rx, ry, rz);

        state->addVector(2.0);
    }

    // Update state by modifiers
    for (unsigned int i = 0; i < modifiers.size(); i++) {
        char mod = modifiers[i];
        switch (mod) {
            case 'F':
                state->fr *= MULTIPLIER;
                break;
            case 'f':
                state->fr /= MULTIPLIER;
                break;
        }
    }
}

void Node::getChildren(SString restOfGenotype) {
    vector <SString> branches = getBranches(restOfGenotype);
    childSize = branches.size();
    for (unsigned int i = 0; i < childSize; i++)
        children.push_back(new Node(branches[i], state));
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

Part *Node::buildModel(Model *model) {
    createPart();
    model->addPart(part);

    for (unsigned int i = 0; i < childSize; i++) {
        Node *child = children[i];
        child->buildModel(model);
        addJointsToModel(model, child, part, child->part);
    }
    return part;
}

void Node::createPart() {
    Part::Shape model_part_type;
    if (part_type == 'E')
        model_part_type = Part::Shape::SHAPE_ELLIPSOID;
    else if (part_type == 'P')
        model_part_type = Part::Shape::SHAPE_CUBOID;
    else if (part_type == 'C')
        model_part_type = Part::Shape::SHAPE_CYLINDER;
    part = new Part(model_part_type);

    part->p = Pt3D(round2(state->location.x),
                   round2(state->location.y),
                   round2(state->location.z)
    );

    if (state->modifierMode) {
        if (state->fr != DEFAULT_FR)
            part->friction = round2(state->fr);
    } else {
        if (params["fr"])
            part->friction = params["fr"];
    }
}

void Node::addJointsToModel(Model *model, Node *child, Part *part, Part *childPart) {
    if (child->joints.empty()) {
        Joint *joint = new Joint();
        joint->shape = Joint::Shape::SHAPE_FIXED;
        joint->attachToParts(part, childPart);
        model->addJoint(joint);
    } else {
        for (auto it = child->joints.begin(); it != child->joints.end(); ++it) {
            Joint *joint = new Joint();
            joint->attachToParts(part, childPart);
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


SString Node::getGeno(SString &result) {
    for (auto it = joints.begin(); it != joints.end(); ++it)
        result += *it;
    for (auto it = modifiers.begin(); it != modifiers.end(); ++it)
        result += *it;
    result += part_type;

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
    return result;
}

void Node::getTree(vector<Node *> &allNodes) {
    allNodes.push_back(this);
    for (unsigned int i = 0; i < childSize; i++)
        children[i]->getTree(allNodes);
}

fS_Genotype::fS_Genotype(const SString &genotype) {
    // M - modifier mode, S - standard mode
    bool modifierMode = genotype[0] == MODIFIER_MODE;
    State *initialState = new State(Pt3D(0), Pt3D(1, 0, 0), modifierMode);
    start_node = new Node(genotype.substr(1, INT_MAX), initialState, true);
}

fS_Genotype::~fS_Genotype() {
    delete start_node;
}

void fS_Genotype::buildModel(Model *model) {
    start_node->buildModel(model);

    // Additional joint
    vector <Node*> allNodes;
    start_node->getTree(allNodes);
    for(unsigned int i=0; i<allNodes.size(); i++) {
        Node *node = allNodes[i];
        if (node->params.find("jd") != node->params.end()) {
            Joint *joint = new Joint();
            Node *otherNode = getNearestNode(allNodes, node);
            joint->attachToParts(node->part, otherNode->part);

            joint->shape = Joint::Shape::SHAPE_FIXED;
            model->addJoint(joint);
        }
    }
}

Node* fS_Genotype::getNearestNode(vector<Node*>allNodes, Node *node){
    Node *result;
    for(unsigned int i=0; i<allNodes.size(); i++){
        Node *otherNode = allNodes[i];
        auto v = node->children;
        if(otherNode != node && find(v.begin(), v.end(), otherNode) != v.end()) {
            result = otherNode;
        }
    }
    return result;
}

SString fS_Genotype::getGeno() {
    SString geno;
    geno.memoryHint(100);     // Provide a small buffer from the start to improve performance
    geno += start_node->state->modifierMode ? SString("M") : SString("S");
    start_node->getGeno(geno);
    return geno;
}

int fS_Genotype::getPartCount() {
    vector < Node * > allNodes;
    start_node->getTree(allNodes);
    return allNodes.size();
}

int fS_Genotype::randomFromRange(int to, int from = 0) {
    return from + rand() % (to - from);
}

double getRandomFromDistribution() {
    return distribution(generator);
}


Node *fS_Genotype::chooseNode(int fromIndex = 0) {
    vector < Node * > allNodes;
    start_node->getTree(allNodes);
    return allNodes[randomFromRange(allNodes.size(), fromIndex)];
}


bool fS_Genotype::addJoint() {
    if (start_node->childSize < 1)
        return false;

    Node *randomNode = chooseNode(1);    // First part does not have joints
    char randomJoint = JOINTS[randomFromRange(JOINT_COUNT, 1)];
    if (randomNode->joints.count(randomJoint) > 0)
        return false;

    randomNode->joints.insert(randomJoint);
    return true;
}


bool fS_Genotype::removeJoint() {
    if (start_node->childSize < 1) // Only one node; there are no joints
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
    if (randomNode->params.count(chosenParam) > 0)
        return false;
    // TODO sensible values for params
    randomNode->params[chosenParam] = 0.5;
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
    Node *newNode = new Node(partType, randomNode->state);
    randomNode->children.push_back(newNode);
    randomNode->childSize += 1;
    return true;
}

bool fS_Genotype::changePartType() {
    Node *randomNode = chooseNode();
    char newType = PART_TYPES[randomFromRange(PART_TYPES.size())];
    if (newType == randomNode->part_type)
        return false;
    randomNode->part_type = newType;
    return true;
}

bool fS_Genotype::addModifier() {
    Node *randomNode = chooseNode();
    char randomModifier = MODIFIERS[randomFromRange(MODIFIERS.length())];
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
    bool result = false;
    int method;
    while(!result) {
        method = GenoOperators::roulette(operations, FS_OPCOUNT);
        switch (method) {
            case 0:
                result = addJoint();
                break;
            case 1:
                result = addParam();
                break;
            case 2:
                result = addPart();
                break;
                // TODO uncomment -- change sometimes not visible in tests
//            case 3:
//                result = changeParam();
//                break;
            case 4:
                result = removeJoint();
                break;
            case 5:
                result = removeParam();
                break;
            case 6:
                result = removePart();
                break;
            case 7:
                result = changePartType();
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

int fS_Genotype::crossOver(char *&g1, char *&g2, float& chg1, float& chg2) {
    fS_Genotype *geno1 = new fS_Genotype(g1);
    fS_Genotype *geno2 = new fS_Genotype(g2);

    if(geno1->start_node->childSize == 0 || geno2->start_node->childSize == 0) {
        delete geno1;
        delete geno2;
        return GENOPER_OPFAIL;
    }


    Node *chosen1 = geno1->chooseNode();
    Node *chosen2 = geno2->chooseNode();
    while(chosen1->childSize == 0)
        chosen1 = geno1->chooseNode();
    while(chosen2->childSize == 0)
        chosen2 = geno2->chooseNode();

    int index1 = randomFromRange(chosen1->childSize);
    int index2 = randomFromRange(chosen2->childSize);

    // Swap
    Node *tmp = chosen1->children[index1];
    chosen1->children[index1] = chosen2->children[index2];
    chosen2->children[index2] = tmp;

    g1 = strdup(geno1->getGeno().c_str());
    g2 = strdup(geno2->getGeno().c_str());

    delete geno1;
    delete geno2;
    return GENOPER_OK;
};
