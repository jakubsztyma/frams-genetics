// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 2019-2020  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FS_OPER_H_
#define _FS_OPER_H_

#include "fS_general.h"
#include "../genooperators.h"


/** @name Codes for general mutation types */
//@{
#define FS_ADD_PART 0
#define FS_REM_PART 1
#define FS_MOD_PART 2
#define FS_CHANGE_JOINT 3
#define FS_ADD_PARAM 4
#define FS_REM_PARAM 5
#define FS_MOD_PARAM 6
#define FS_MOD_MOD 7
#define FS_ADD_NEURO 8
#define FS_REM_NEURO 9
#define FS_MOD_NEURO_CONNECTION 10
#define FS_ADD_NEURO_CONNECTION 11
#define FS_REM_NEURO_CONNECTION 12
#define FS_MOD_NEURO_PARAMS 13
#define FS_OPCOUNT 14
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
	bool addPart(fS_Genotype &geno, const vector<Part::Shape> &availablePartShapes, bool mutateSize = true);

	/**
	 * Performs remove part type mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool removePart(fS_Genotype &geno);

	/**
	 * Performs change part type mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool changePartType(fS_Genotype &geno, const vector<Part::Shape> &availablePartShapes);

	/**
	 * Changes the type of one joint in genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool changeJoint(fS_Genotype &geno);

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
	 * Changes the value of specified parameter.
	 * The state of the node must be previously calculated
	 * @param node - the node on which parameter is modified
	 * @param key - the key of parameter
	 * @return
	 */
	bool mutateParamValue(Node *node, string key);

	/**
	 * Performs change modifier mutation on genotype
	 * @return true if mutation succeeded, false otherwise
	 */
	bool changeModifier(fS_Genotype &geno);

	bool addNeuro(fS_Genotype &geno);

	bool removeNeuro(fS_Genotype &geno);

	bool changeNeuroConnection(fS_Genotype &geno);

	bool addNeuroConnection(fS_Genotype &geno);

	bool removeNeuroConnection(fS_Genotype &geno);

	bool changeNeuroParam(fS_Genotype &geno);

	/**
	 * Change the value of the scale parameter by given multiplier
	 * Do not change the value if any of the scale restrictions is not satisfied
	 * @param paramKey
	 * @param multiplier
	 * @param ensureCircleSection
	 * @return True if the parameter value was change, false otherwise
	 */
	bool mutateScaleParam(Node *node, string key, bool ensureCircleSection);
};

#endif
