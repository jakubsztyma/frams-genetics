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
    bool modifierMode;
    double fr;
    State(State *_state);
    State(Pt3D _location, Pt3D _v, bool modifierMode);
    void addVector(double length);
    void rotate(double rx, double ry, double rz);
};

class Node{
private:
    bool isStart;
    char part_type;
    map<string, double> params;
    vector<Node*> children;
    set<char> joints;
    vector<char> modifiers;

    SString extractModifiers(SString restOfGenotype);
    SString extractPartType(SString restOfGenotype);
    SString extractParams(SString restOfGenotype);
    vector<SString> getBranches(SString restOfGenotype);
    void getState(State *_state);
    void getChildren(SString restOfGenotype);
    void setParamsOnPart(Part *part);
    void addJointsToModel(Model *model, Node *child, Part *part, Part *childPart);
public:
    State *state;
    Node(const SString &genotype, State *state, bool _isStart);
    Part* buildModel(Model *model);
    SString getGeno();
    vector<Node> getTree();
};

class fS_Genotype{
public:
    int  getPartCount();
    int chooseIndex(int length);
    Node chooseNode(int fromIndex);

    void randomFromDistribution();
    void addPart();
    void addJoint();
    void removePart();
    void removeJoint();
    void addParameter();
    void removeParameter();
    void changeParameter();

    Node *start_node;

    fS_Genotype(const SString &genotype);
    ~fS_Genotype(){};

    string toString();
    void buildModel(Model *model);
    SString getGeno();

    void mutate();
    void crossover();
};


#endif //CPP_FS_CONV_H
