// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FB_OPER_H_
#define _FB_OPER_H_

#include "../genooperators.h"
#include <list>

/** @name Codes for general fB mutation types */
//@{
#define FB_SUBSTITUTION  0 ///<Relative probability of mutation by changing single random letter in genotype (substitution)
#define FB_INSERTION     1 ///<Relative probability of mutation by inserting characters in random place of genotype
#define FB_NCLASSINS     2 ///<Relative probability of mutation by inserting neuron class definition in random place of genotype
#define FB_DELETION      3 ///<Relative probability of mutation by deleting random characters in genotype
#define FB_DUPLICATION   4 ///<Relative probability of mutation by copying single *gene* of genotype and appending it to the beginning of this genotype
#define FB_TRANSLOCATION 5 ///<Relative probability of mutation by replacing two substrings in genotype
#define FB_MUT_COUNT     6 ///<Count of mutation types
//@}

/** @name Codes for fB cross over types */
//@{
#define FB_GENE_TRANSFER 0 ///<Relative probability of crossing over by transferring single genes from both parents to beginning of each other
#define FB_CROSSING_OVER 1 ///<Relative probability of crossing over by random distribution of genes from both parents to both children
#define FB_XOVER_COUNT   2 ///<Count of crossing over types
//@}

class Geno_fB : public GenoOperators
{
private:
	bool hasStick(const SString &genotype);
	SString detokenizeSequence(std::list<SString> *tokenlist);
	std::list<SString> tokenizeSequence(const SString &genotype);

public:
	double mutationprobs[FB_MUT_COUNT];
	double crossoverprobs[FB_XOVER_COUNT];

	Geno_fB();

	int checkValidity(const char *geno, const char *genoname);

	int validate(char *&geno, const char *genoname);

	int mutate(char *&geno, float& chg, int &method);

	int crossOver(char *&g1, char *&g2, float& chg1, float& chg2);

	virtual const char* getSimplest() { return "5\naaazz"; }

	uint32_t style(const char *geno, int pos);
};

#endif //_FB_OPER_H_
