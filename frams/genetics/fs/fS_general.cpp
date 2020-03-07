//
// Created by jakub on 21.02.2020.
//

#include <iostream>
#include <stdlib.h>
#include<bits/stdc++.h>
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
#define PART_TYPES "EPC"
#define MULTIPLIER 1.1

string JOINTS = "abcd";
string MODIFIERS = "xyz";

double DEFAULT_FR = 0.4;

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
    SString jointTypes = restOfGenotype.substr(0, partTypePosition);

    for (int i = 0; i < jointTypes.len(); i++) {
        // Extract modifiers and joints
        char jType = jointTypes[i];
        if (JOINTS.find(jType) != string::npos)
            joints.insert(jType);
        else
            modifiers.push_back(jType);
    }
    if (joints.empty()) // Add default joint if no others were specified
        joints.insert(DEFAULT_JOINT);
    return restOfGenotype.substr(partTypePosition, INT_MAX);
}

SString Node::extractPartType(SString restOfGenotype) {
    switch (restOfGenotype[0]) {
        case 'E':
            part_type = Part::Shape::SHAPE_ELLIPSOID;
            break;
        case 'P':
            part_type = Part::Shape::SHAPE_CUBOID;
            break;
        case 'C':
            part_type = Part::Shape::SHAPE_CYLINDER;
            break;
    }
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
    Part *part = new Part(part_type);
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
    for (set<char>::iterator it = child->joints.begin(); it != child->joints.end(); ++it) {
        Joint *joint = new Joint();
        joint->attachToParts(part, childPart);
        switch (*it) {
            case 'a':
                joint->shape = Joint::Shape::SHAPE_FIXED;
                break;
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

fS_Genotype::fS_Genotype(const SString &genotype) {
    // M - modifier mode, S - standard mode
    bool modifierMode = genotype[0] == MODIFIER_MODE;
    State *initialState = new State(Pt3D(0), Pt3D(1, 0, 0), modifierMode);
    start_node = new Node(genotype.substr(1, INT_MAX), initialState, true);
}

void fS_Genotype::buildModel(Model *model) {
    start_node->buildModel(model);
}
