//
// Created by jakub on 21.02.2020.
//

#ifndef CPP_FS_CONV_H
#define CPP_FS_CONV_H

#include <iostream>
#include <vector>
#include "frams/util/sstring.h"
#include "frams/model/model.h"

using namespace std;

class Node{
private:
    SString rest;
    Part::Shape part_type;
    vector<Node*> children;
    vector<SString> getBranches();
public:

    Node(const SString &genotype);
    //void init(const SString &genotype);
    Part* buildModel(Model *model);
    void get_children();
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
