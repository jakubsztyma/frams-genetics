//
// Created by jakub on 21.02.2020.
//

#ifndef CPP_FS_CONV_H
#define CPP_FS_CONV_H

#include <iostream>
#include <vector>
#include <map>
#include "frams/util/sstring.h"
#include "frams/model/model.h"

using namespace std;

class Node{
private:
    Part::Shape part_type;
    map<string, float> params;
    vector<Node*> children;
    vector<SString> getBranches(SString restOfGenotype);
public:

    Node(const SString &genotype);
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
