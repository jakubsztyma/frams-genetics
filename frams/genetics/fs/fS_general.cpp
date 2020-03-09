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

#define PART_TYPES "EPC"
#define DEFAULT_JOINT 'a'
#define MULTIPLIER 1.1
#define JOINT_COUNT 4
#define DEFAULT_FR 0.4

const string JOINTS = "abcd";
const string OTHER_JOINTS = "bcd";
const string MODIFIERS = "xyz";

double round2(double var) {
    double value = (int) (var * 100 + .5);
    return (double) value / 100;
}

vector <SString> split(SString str, char delim) {
    vector <SString> cont;
    int index = 0, new_index;
    while (true) {
        new_index = str.indexOf(delim, index);
        if (new_index == -1) {
            cont.push_back(str.substr(index, INT_MAX));
            break;
        }
        cont.push_back(str.substr(index, new_index - index));
        index = new_index + 1;
    }
    return cont;
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

SString Node::extractModifiers(SString restOfGenotype) {
    smatch m;
    string s(restOfGenotype.c_str());
    regex_search(s, m, regex("E|P|C"));
    int partTypePosition = m.position();
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
    vector <SString> keyValuePairs = split(paramString, PARAM_SEPARATOR);
    for (unsigned int i = 0; i < keyValuePairs.size(); i++) {
        vector <SString> keyValue = split(keyValuePairs[i], PARAM_KEY_VALUE_SEPARATOR);
        // TODO handle wrong length exception
        double value = atof(keyValue[1].c_str());
        params[keyValue[0].c_str()] = value;
    }

    return restOfGenotype.substr(paramsEndIndex + 1, INT_MAX);
}

void Node::getState(State *_state) {
    if (isStart) {
        state = _state;
    } else {
        state = new State(_state);
        state->rotate(params["rx"], params["ry"], params["rz"]);
        if (params["rx"] == 0.)
            params.erase("rx");
        if (params["ry"] == 0.)
            params.erase("ry");
        if (params["rz"] == 0.)
            params.erase("rz");
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
    for (unsigned int i = 0; i < branches.size(); i++)
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
    Part::Shape model_part_type;
    if (part_type == 'E')
        model_part_type = Part::Shape::SHAPE_ELLIPSOID;
    else if (part_type == 'P')
        model_part_type = Part::Shape::SHAPE_CUBOID;
    else if (part_type == 'C')
        model_part_type = Part::Shape::SHAPE_CYLINDER;
    Part *part = new Part(model_part_type);
    setParamsOnPart(part);
    model->addPart(part);

    for (unsigned int i = 0; i < children.size(); i++) {
        Node *child = children[i];
        Part *childPart = child->buildModel(model);
        addJointsToModel(model, child, part, childPart);
    }
    return part;
}

void Node::setParamsOnPart(Part *part) {
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
        for (set<char>::iterator it = child->joints.begin(); it != child->joints.end(); ++it) {
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


SString Node::getGeno() {
    SString result = "";
    for (set<char>::iterator it = joints.begin(); it != joints.end(); ++it) {
        result += *it;
    }
    for (vector<char>::iterator it = modifiers.begin(); it != modifiers.end(); ++it)
        result += *it;
    result += part_type;

    if (params.size() > 0) {
        result += PARAM_START;
        for (map<string, double>::iterator it = params.begin(); it != params.end(); it++) {
            result += it->first.c_str();
            result += PARAM_KEY_VALUE_SEPARATOR;
            string value_text = to_string(it->second);
            result += value_text.substr(0, value_text.find(".") + 2).c_str(); // ROund
            result += PARAM_SEPARATOR;
        }
        result = result.substr(0, result.len() - 1);
        result += PARAM_END;
    }

    unsigned int children_size = children.size();
    if (children_size == 1)
        result += children[0]->getGeno();
    else if (children_size > 1) {
        result += BRANCH_START;
        for (unsigned int i = 0; i < children_size - 1; i++) {
            result += children[i]->getGeno();
            result += BRANCH_SEPARATOR;
        }
        result += children[children_size - 1]->getGeno();
        result += BRANCH_END;
    }
    return result;
}

vector <Node*> Node::getTree() {
    vector <Node*> allNodes;
    allNodes.push_back(this);
    for (unsigned int i = 0; i < children.size(); i++) {
        vector <Node*> offspring = children[i]->getTree();
        allNodes.reserve(allNodes.size() + distance(offspring.begin(), offspring.end())); // Improve performance
        allNodes.insert(allNodes.end(), offspring.begin(), offspring.end());
    }
    return allNodes;
}

fS_Genotype::fS_Genotype(const SString &genotype) {
    // M - modifier mode, S - standard mode
    bool modifierMode = genotype[0] == MODIFIER_MODE;
    State *initialState = new State(Pt3D(0), Pt3D(1, 0, 0), modifierMode);
    start_node = new Node(genotype.substr(1, INT_MAX), initialState, true);
}

void fS_Genotype::buildModel(Model *model) {
    start_node->buildModel(model);
}

SString fS_Genotype::getGeno() {
    SString mode = start_node->state->modifierMode ? SString("M") : SString("S");
    SString actualGeno = start_node->getGeno();
    return mode + actualGeno;
}

int fS_Genotype::getPartCount() {
    return start_node->getTree().size();
}

int fS_Genotype::randomFromRange(int to, int from = 0) {
    return from + rand() % (to - from);
}


Node* fS_Genotype::chooseNode(int fromIndex = 0) {
    vector <Node*> allNodes = start_node->getTree();
    return allNodes[randomFromRange(allNodes.size(), fromIndex)];
}


bool fS_Genotype::addJoint() {
    if(start_node->children.size() < 1)
        return false;

    Node *randomNode = chooseNode(1);    // First part does not have joints
    char randomJoint = JOINTS[randomFromRange(JOINT_COUNT, 1)];
    if (randomNode->joints.count(randomJoint) != 0)
        return false;
    
    randomNode->joints.insert(randomJoint);
    return true;
}


bool fS_Genotype::removeJoint(){
    if(start_node->children.size() < 1)
        return false;

    Node *randomNode = chooseNode(1);    // First part does not have joints
    int jointsCount = randomNode->joints.size();
    if (jointsCount < 1)
        return false;
    int index = *(randomNode->joints.begin()) + randomFromRange(jointsCount);
    randomNode->joints.erase(index);

    return true;
}
