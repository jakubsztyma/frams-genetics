//
// Created by jakub on 21.02.2020.
//

#ifndef CPP_FS_OPER_H
#define CPP_FS_OPER_H

#include "../genooperators.h"

using namespace std;

class fS_Operators : public GenoOperators{
public:
    int crossOver(char *&g1, char *&g2, float& chg1, float& chg2);

    int checkValidity(const char *geno, const char *genoname);

    int mutate(char *&geno, float& chg, int &method);
};

#endif //CPP_FS_OPER_H
