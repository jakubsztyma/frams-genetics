//
// Created by jakub on 22.02.2020.
//

#include "fS_conv.h"

using namespace std;

SString GenoConv_fS0::convert(SString &i) {
    fS_Genotype genotype = fS_Genotype();
    genotype.parseGenotype(i);
    return genotype.buildModel();
}
