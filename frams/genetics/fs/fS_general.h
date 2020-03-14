//
// Created by jakub on 21.02.2020.
//

#ifndef CPP_FS_CONV_H
#define CPP_FS_CONV_H

#define FS_OPCOUNT 10

#include <iostream>
#include <vector>
#include <map>
#include <regex>
#include <set>
#include <iterator>
#include "common/Convert.h"
#include "frams/genetics/genooperators.h"
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
    friend class fS_Genotype;
private:
public:
    bool isStart;
    char part_type;
    Part *part;
    unsigned int childSize = 0;
    int getPartPosition(SString restOfGenotype);
    map<string, double> params;
    vector<Node*> children;
    vector<char> modifiers;
    set<char> joints;

    SString extractModifiers(SString restOfGenotype);
    SString extractPartType(SString restOfGenotype);
    SString extractParams(SString restOfGenotype);
    vector<SString> getBranches(SString restOfGenotype);
    void getState(State *_state);
    void getChildren(SString restOfGenotype);
    void createPart();
    void addJointsToModel(Model *model, Node *child, Part *part, Part *childPart);
    void getTree(vector<Node*> &allNodes);
    Part* buildModel(Model *model);
//public:
    State *state;

    Node(const SString &genotype, State *state, bool _isStart);
    ~Node();
    SString getGeno(SString &result);
};

class fS_Genotype{
    friend class Node;

private:
public:
    Node *start_node;

    vector<Node*> getTree();
    Node* chooseNode(int fromIndex);
    int randomFromRange(int to, int from);
    void randomFromDistribution();
    Node *getNearestNode(vector<Node*>allNodes, Node *node);
//public:
    int  getPartCount();

    fS_Genotype(const SString &genotype);
    ~fS_Genotype();

    void buildModel(Model *model);
    SString getGeno();

    bool addJoint();
    bool removeJoint();
    bool addPart();
    bool changePartType();
    bool removePart();
    bool addParam();
    bool removeParam();
    bool changeParam();
    bool addModifier();
    bool removeModifier();

    void mutate();
    int crossOver(char *&g1, char *&g2, float& chg1, float& chg2);
};


#endif //CPP_FS_CONV_H
