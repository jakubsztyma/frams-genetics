//
// Created by jakub on 21.02.2020.
//

#ifndef CPP_FS_CONV_H
#define CPP_FS_CONV_H

#include <iostream>
#include <vector>
#include <map>
#include "frams/util/3d.h"
#include "frams/util/sstring.h"
#include "frams/model/model.h"

using namespace std;

class State{
public:
    double x, y, z, vx, vy, vz;
    State(double _x, double _y, double _z, double _vx, double _vy, double _vz):
    x(_x), y(_y), z(_z), vx(_vx), vy(_vy), vz(_vz) {}
    void addVector(double length);
    void rotate();
};

class Node{
private:
    bool isStart;
    Part::Shape part_type;
    map<string, float> params;
    vector<Node*> children;
    vector<SString> getBranches(SString restOfGenotype);
    void getState(State *_state);
public:
    State *state;
    Node(const SString &genotype, State *state, bool _isStart);
    Part* buildModel(Model *model);
    void getChildren(SString restOfGenotype);
    SString extractParams(SString restOfGenotype);
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
