//
// Created by jakub on 22.02.2020.
//

#include "fS_conv.h"
#include "frams/util/multimap.h"

using namespace std;

SString GenoConv_fS0::convert(SString &i) {
    Model *model = new Model();
    model->open(false);

    fS_Genotype genotype = fS_Genotype(i);
    genotype.buildModel(model);

    MultiMap map;
    model->getCurrentToF0Map(map);
    model->close();
    return model->getF0Geno().getGenes();
}
