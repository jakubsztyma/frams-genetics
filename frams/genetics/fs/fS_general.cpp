//
// Created by jakub on 21.02.2020.
//

#include <iostream>
#include <stdlib.h>
#include "fS_general.h"
#include "frams/util/multimap.h"

using namespace std;

char BRANCH_START = '(';
char BRANCH_END = ')';
char BRANCH_SEPARATOR = ',';
char PARAM_START = '{';
char PARAM_END = '}';
char PARAM_SEPARATOR = ';';
char DEFAULT_JOINT = 'a';
string PART_TYPES = "EPC";
string OTHER_JOINTS = "bcd";
string MODIFIERS = "xyz";

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
            cont.push_back(str.substr(index, str.len()));
            break;
        }
        cont.push_back(str.substr(index, new_index - index));
        index = new_index + 1;
    }
    return cont;
}

State::State(Pt3D _location, Pt3D _v) {
    location = Pt3D(_location);
    v = Pt3D(_v);
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
    SString restOfGenotype = extractJoints(genotype);
    restOfGenotype = extractPartType(restOfGenotype);
    if (restOfGenotype.len() > 0 && restOfGenotype[0] == PARAM_START)
        restOfGenotype = extractParams(restOfGenotype);
    getState(_state);
    if (restOfGenotype.len() > 0)
        getChildren(restOfGenotype);
}

SString Node::extractJoints(SString restOfGenotype){
    smatch m;
    string s(restOfGenotype.c_str());
    regex_search(s, m, regex("E|P|C"));
    int partTypePosition = m.position();
    SString jTypes = restOfGenotype.substr(0, partTypePosition);
    cout<<jTypes.c_str()<<endl;
    for(int i=0; i<jTypes.len(); i++)
        jointTypes.insert(jTypes[i]);
    if(jointTypes.empty())
        jointTypes.insert(DEFAULT_JOINT);
    return restOfGenotype.substr(partTypePosition, restOfGenotype.len());
}

SString Node::extractPartType(SString restOfGenotype){
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
    return restOfGenotype.substr(1, restOfGenotype.len());
}

SString Node::extractParams(SString restOfGenotype) {
    int paramsEndIndex = restOfGenotype.indexOf('}');
    SString paramString = restOfGenotype.substr(1, paramsEndIndex - 1);
    vector <SString> keyValuePairs = split(paramString, PARAM_SEPARATOR);
    for (unsigned int i = 0; i < keyValuePairs.size(); i++) {
        vector <SString> keyValue = split(keyValuePairs[i], '=');
        // TODO handle wrong length exception
        double value = atof(keyValue[1].c_str());
        params[keyValue[0].c_str()] = value;
    }

    return restOfGenotype.substr(paramsEndIndex + 1, restOfGenotype.len());
}

void Node::getState(State *_state) {
    if (isStart) {
        state = _state;
    } else {
        state = new State(_state->location, _state->v);
        state->rotate(params["rx"], params["ry"], params["rz"]);
        state->addVector(2.0);
    }
}

void Node::getChildren(SString restOfGenotype) {
    vector <SString> branches = getBranches(restOfGenotype);
    for (unsigned int i = 0; i < branches.size(); i++) {
        Node *child_node = new Node(branches[i], state);
        children.push_back(child_node);
    }
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
    for (int i = 0; i < restOfGenotype.len(); i++) {
        char c = restOfGenotype[i];
        if (c == BRANCH_START)
            depth += 1;
        else if ((c == BRANCH_SEPARATOR && depth == 1) || i + 1 == restOfGenotype.len()) {
            children.push_back(restOfGenotype.substr(start, i - start));
            start = i + 1;
        } else if (c == BRANCH_END)
            depth -= 1;
    }
    return children;
}

Part *Node::buildModel(Model *model) {
    Part *part = new Part(part_type);

    state->location.x = round2(state->location.x);
    state->location.y = round2(state->location.y);
    state->location.z = round2(state->location.z);
    part->p = Pt3D(state->location);
    // TODO debug why params are not working
    if (params["m"]) {
        part->mass = params["m"];
    }
    model->addPart(part);

    for (unsigned int i = 0; i < children.size(); i++) {
        Node *child = children[i];
        Part *childPart = child->buildModel(model);
        addJointsToModel(model, child, part, childPart);
    }
    return part;
}

void Node::addJointsToModel(Model *model, Node *child, Part *part, Part *childPart){
    for (set<char>::iterator it=child->jointTypes.begin(); it!=child->jointTypes.end(); ++it) {
        Joint *joint = new Joint();
        joint->attachToParts(part, childPart);
        joint->shape = Joint::Shape::SHAPE_FIXED;
        model->addJoint(joint);
    }
}

int fS_Genotype::parseGenotype(const SString &genotype) {
    State *initialState = new State(Pt3D(0), Pt3D(1, 0, 0));
    start_node = new Node(genotype, initialState, true);
    return 0;
}

SString fS_Genotype::buildModel() {
    Model *model = new Model();
    model->open(false);

    start_node->buildModel(model);
    //TODO check why multiple joints don't show
    //cout<<model->getPartCount()<<" "<<model->getJointCount()<<endl;

    MultiMap map;
    model->getCurrentToF0Map(map);
    model->close();
    return model->getF0Geno().getGenes();
}
