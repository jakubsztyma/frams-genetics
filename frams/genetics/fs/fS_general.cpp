//
// Created by jakub on 21.02.2020.
//

#include <iostream>
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
        cout<<index<<new_index<<str.substr(index, new_index - index).c_str()<<endl;
        cont.push_back(str.substr(index, new_index - index));
        index = new_index + 1;
    }
    return cont;
}

Node::Node(const SString &genotype) {
    rest = genotype.substr(1, genotype.len() - 1);
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
    if(rest.len() > 0)
        get_children();
}

void Node::get_children() {
    vector<SString> branches = getBranches();
    cout<<branches.size()<<endl;
    for(unsigned int i=0; i<branches.size(); i++){
        cout<<branches[i].c_str()<<endl;
        Node *child_node = new Node(branches[i]);
        children.push_back(child_node);
    }
}

vector<SString> Node::getBranches() {
    if(rest[0] != BRANCH_START){
        vector<SString> result{rest};  // Only one child
        return result;
    }
//    if(sequence[-1] != BRANCH_END) {
//        raise
//        InvalidGenotypeException(f
//        'Missing "{self.BRANCH_END}" sign')
//    }

    int depth = 0;
    int start = 1;
    vector<SString> children;
    for(int i=0; i<rest.len(); i++){
        char c = rest[i];
        if(c == BRANCH_START)
            depth += 1;
        else if((c == BRANCH_SEPARATOR && depth == 1) || i + 1 == rest.len()){
            children.push_back(rest.substr(start, i - start));
            start = i + 1;
        }
        else if(c == BRANCH_END)
            depth -= 1;
        }
    return children;
}

Part* Node::buildModel(Model *model) {
    Part *newpart = new Part(part_type);
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
