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

vector<SString> split(SString str, char delim)
{
    vector<SString> cont;
    int index = 0, new_index;
    while(true){
        new_index = str.indexOf(delim, index);
        if(new_index == -1) {
            cont.push_back(str.substr(index, str.len()));
            break;
        }
        cont.push_back(str.substr(index, new_index - index));
        index = new_index + 1;
    }
    return cont;
}

Node::Node(const SString &genotype) {
    switch (genotype[0]) {
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
    SString restOfGenotype = genotype.substr(1, genotype.len());
    if(restOfGenotype.len() > 0 && restOfGenotype[0] == '{')
        restOfGenotype = extractParams(restOfGenotype);
    if(restOfGenotype.len() > 0)
        getChildren(restOfGenotype);
}

SString Node::extractParams(SString restOfGenotype){
    int paramsEndIndex = restOfGenotype.indexOf('}');
    SString paramString = restOfGenotype.substr(1, paramsEndIndex - 1);
    vector<SString> keyValuePairs = split(paramString, PARAM_SEPARATOR);
    for(unsigned int i=0; i<keyValuePairs.size(); i++){
        vector<SString> keyValue = split(keyValuePairs[i], '=');
        // TODO handle wrong length exception
        string key = keyValue[0].c_str();
        float value = atof(keyValue[1].c_str());
        params.insert(pair<string, float>(key, value));
    }

    return restOfGenotype.substr(paramsEndIndex + 1, restOfGenotype.len());
}

void Node::getChildren(SString restOfGenotype) {
    vector<SString> branches = getBranches(restOfGenotype);
    for(unsigned int i=0; i<branches.size(); i++){
        Node *child_node = new Node(branches[i]);
        children.push_back(child_node);
    }
}

vector<SString> Node::getBranches(SString restOfGenotype) {
    if(restOfGenotype[0] != BRANCH_START){
        vector<SString> result{restOfGenotype};  // Only one child
        return result;
    }
    // TODO handle wrong syntax

    int depth = 0;
    int start = 1;
    vector<SString> children;
    for(int i=0; i<restOfGenotype.len(); i++){
        char c = restOfGenotype[i];
        if(c == BRANCH_START)
            depth += 1;
        else if((c == BRANCH_SEPARATOR && depth == 1) || i + 1 == restOfGenotype.len()){
            children.push_back(restOfGenotype.substr(start, i - start));
            start = i + 1;
        }
        else if(c == BRANCH_END)
            depth -= 1;
        }
    return children;
}

Part* Node::buildModel(Model *model) {
    Part *newpart = new Part(part_type);
    // TODO debug why params are not working
    if(params["m"]) {
        newpart->mass = params["m"];
    }
    model->addPart(newpart);

    for(unsigned int i=0; i<children.size(); i++){
        Part *childPart = children[i]->buildModel(model);

        Joint *joint = new Joint();
        joint->attachToParts(newpart, childPart);
        joint->shape = Joint::Shape::SHAPE_FIXED;
        model->addJoint(joint);
    }
    return newpart;
}

int fS_Genotype::parseGenotype(const SString &genotype) {
    start_node = new Node(genotype);
    return 0;
}

SString fS_Genotype::buildModel() {
    Model *model = new Model();
    model->open(false);

    start_node->buildModel(model);

    MultiMap map;
    model->getCurrentToF0Map(map);
    model->close();
    return model->getF0Geno().getGenes();
}
