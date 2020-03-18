//
// Created by jakub on 21.02.2020.
//

#ifndef CPP_FS_OPER_H
#define CPP_FS_OPER_H

class fS_Operators{
public:
    int mutate(char *&geno, float& chg, int &method){
        fS_Genotype genotype(geno);
        genotype.mutate();

        free(geno);
        geno = strdup(genotype.getGeno().c_str());
        return GENOPER_OK;
    }

    int crossOver(char *&g1, char *&g2, float& chg1, float& chg2);
};

#endif //CPP_FS_OPER_H
