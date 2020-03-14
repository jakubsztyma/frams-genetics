//
// Created by jakub on 22.02.2020.
//

#include "fS_conv.h"

using namespace std;

SString GenoConv_fS0::convert(SString &i, MultiMap *map, bool using_checkpoints=false) {
    Model *model = new Model();
    model->open(false);

    fS_Genotype genotype = fS_Genotype(i);
    genotype.buildModel(model);

    model->getCurrentToF0Map(*map);
    model->close();
    SString genes = model->getF0Geno().getGenes();
    delete model;
    return genes;
}
