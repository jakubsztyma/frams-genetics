//
// Created by jakub on 22.02.2020.
//

#include "fS_conv.h"

using namespace std;

SString GenoConv_fS0::convert(SString &i, MultiMap *map, bool using_checkpoints) {
    fS_Genotype *genotype;
    try {
        genotype = new fS_Genotype(i);
    }
    catch (const char *msg){
        return SString();
    }

    Model *model = new Model();
    model->open(using_checkpoints);
    genotype->buildModel(model);
    model->getCurrentToF0Map(*map);
    model->close();
    SString genes = model->getF0Geno().getGenes();
    delete genotype;
    delete model;
    return genes;
}
