//
// Created by jakub on 21.02.2020.
//

#ifndef _FS_OPER_H_
#define _FS_OPER_H_

#include "fS_general.h"
#include "../genooperators.h"

/** @name Number of various genetic operations */
#define FS_OPCOUNT 16

/** @name Codes for general mutation types */
//@{
#define FS_ADD_PART 0
#define FS_REM_PART 1
#define FS_MOD_PART 2
#define FS_ADD_JOINT 3
#define FS_REM_JOINT 4
#define FS_ADD_PARAM 5
#define FS_REM_PARAM 6
#define FS_MOD_PARAM 7
#define FS_ADD_MOD 8
#define FS_REM_MOD 9
#define FS_ADD_NEURO 10
#define FS_REM_NEURO 11
#define FS_MOD_NEURO_CONNECTION 12
#define FS_ADD_NEURO_CONNECTION 13
#define FS_REM_NEURO_CONNECTION 14
#define FS_MOD_NEURO_PARAMS 15
//@}

class fS_Operators : public GenoOperators
{
public:
	fS_Operators();

	double prob[FS_OPCOUNT];
	int ensureCircleSection;

	int crossOver(char *&g1, char *&g2, float &chg1, float &chg2);

	int checkValidity(const char *geno, const char *genoname);

	int mutate(char *&geno, float &chg, int &method);
};

#endif
