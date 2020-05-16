//
// Created by jakub on 19.03.2020.
//

#include "fS_general.h"
#include "fS_oper.h"

using namespace std;

int fS_Operators::checkValidity(const char *geno, const char *genoname) {
    try {
        fS_Genotype genotype = fS_Genotype(geno);
    }
    catch (const char *msg){
        return 1;
    }
    return 0;
}


int fS_Operators::mutate(char *&geno, float& chg, int &method){
    fS_Genotype genotype(geno);
    genotype.mutate(method);

    free(geno);
    geno = strdup(genotype.getGeno().c_str());
    return GENOPER_OK;
}

int fS_Operators::crossOver(char *&g1, char *&g2, float &chg1, float &chg2) {
    int parentCount = 2;
    fS_Genotype *parents[parentCount] = {new fS_Genotype(g1), new fS_Genotype(g2)};

    if (parents[0]->startNode->childSize == 0 || parents[1]->startNode->childSize == 0) {
        delete parents[0];
        delete parents[1];
        return GENOPER_OPFAIL;
    }

    Node *chosen[parentCount];
    int indexes[2];
    for (int i = 0; i < parentCount; i++) {
        vector < Node * > allNodes = parents[i]->getAllNodes();
        do {
            chosen[i] = allNodes[parents[i]->randomFromRange(allNodes.size(), 0)];
        } while (chosen[i]->childSize == 0);
        indexes[i] = parents[i]->randomFromRange(chosen[i]->childSize, 0);
    }
    swap(chosen[0]->children[indexes[0]], chosen[1]->children[indexes[1]]);

    free(g1);
    free(g2);
    g1 = strdup(parents[0]->getGeno().c_str());
    g2 = strdup(parents[1]->getGeno().c_str());

    delete parents[0];
    delete parents[1];
    return GENOPER_OK;
}