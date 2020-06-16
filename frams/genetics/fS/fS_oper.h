// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 2019-2020  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

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


const int PARENT_COUNT = 2;

class GenoOper_fS : public GenoOperators
{
public:
	static const int crossOverTries = 100;
	double prob[FS_OPCOUNT];
	paInt ensureCircleSection;
	paInt useElli, useCub,  useCyl;
	paInt strongAddPart;

	GenoOper_fS();

	int crossOver(char *&g1, char *&g2, float &chg1, float &chg2);

	int checkValidity(const char *geno, const char *genoname);

	int mutate(char *&geno, float &chg, int &method);

	uint32_t style(const char *g, int pos);

	const char* getSimplest();

	/**
	 * Remove connections to the subtree that will be removed from genotype
	 * @param geno An fS_Genotype
	 * @param sub A subtree that will be removed from genotype
	 * @param subStart An index of the first neuron in the removed genotype
	 */
	void rearrangeConnectionsBeforeCrossover(fS_Genotype *geno, Node *sub, int &subStart);

	/**
	 *
	 * @param geno An fS_Genotype
	 * @param sub A subtree that was added to genotype
	 * @param subOldStart An index of the first neuron in the subtree when it was in old genotype
	 */
	void rearrangeConnectionsAfterCrossover(fS_Genotype *geno, Node *sub, int subOldStart);


	/**
	 * Performs add part mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool addPart(fS_Genotype &geno, string availableTypes = "ECR", bool mutateSize = true);

	/**
	 * Performs remove part type mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool removePart(fS_Genotype &geno);

	/**
	 * Performs change part type mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool changePartType(fS_Genotype &geno, string availableTypes = "ECR");

	/**
	 * Performs add joint mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool addJoint(fS_Genotype &geno);

	/**
	 * Performs remove mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool removeJoint(fS_Genotype &geno);

	/**
	 * Performs add param mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool addParam(fS_Genotype &geno);

	/**
	 * Performs remove param mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool removeParam(fS_Genotype &geno);

	/**
	 * Performs change param mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool changeParam(fS_Genotype &geno);

	/**
	 * Performs add modifier mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool addModifier(fS_Genotype &geno);

	/**
	 * Performs remove modifier mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool removeModifier(fS_Genotype &geno);

	bool addNeuro(fS_Genotype &geno);

	bool removeNeuro(fS_Genotype &geno);

	bool changeNeuroConnection(fS_Genotype &geno);

	bool addNeuroConnection(fS_Genotype &geno);

	bool removeNeuroConnection(fS_Genotype &geno);

	bool changeNeuroParam(fS_Genotype &geno);
};

#endif
