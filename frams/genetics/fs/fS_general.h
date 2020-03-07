//
// Created by jakub on 21.02.2020.
//

#ifndef CPP_FS_CONV_H
#define CPP_FS_CONV_H

#include <iostream>
#include <vector>
#include <map>
#include <regex>
#include <set>
#include "common/Convert.h"
#include "frams/util/3d.h"
#include "frams/util/sstring.h"
#include "frams/model/model.h"

using namespace std;

class State{
public:
    Pt3D location, v;
    State(Pt3D _location, Pt3D _v);
    void addVector(double length);
    void rotate(double rx, double ry, double rz);
};

class Node{
private:
    bool isStart;
    Part::Shape part_type;
    map<string, double> params;
    vector<Node*> children;
    set<char> jointTypes;

    SString extractJoints(SString restOfGenotype);
    SString extractPartType(SString restOfGenotype);
    SString extractParams(SString restOfGenotype);
    vector<SString> getBranches(SString restOfGenotype);
    void getState(State *_state);
    void getChildren(SString restOfGenotype);
    void addJointsToModel(Model *model, Node *child, Part *part, Part *childPart);
public:
    State *state;
    Node(const SString &genotype, State *state, bool _isStart);
    Part* buildModel(Model *model);
};

class fS_Genotype{
public:
    Node *start_node;

    fS_Genotype(){};
    ~fS_Genotype(){};

    int parseGenotype(const SString &genotype);
    string toString();
    SString buildModel();
};


#endif //CPP_FS_CONV_H
