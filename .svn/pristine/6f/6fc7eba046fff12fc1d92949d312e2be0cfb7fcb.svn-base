// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2017  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

// Copyright (C) 1999,2000  Adam Rotaru-Varga (adam_rotaru@yahoo.com), GNU LGPL
// Copyright (C) since 2001 Maciej Komosinski

#ifndef _F4_OPER_H_
#define _F4_OPER_H_

#include <stdio.h>
#include "f4_general.h"
#include "common/nonstd.h"
#include "../genooperators.h"
#include <frams/param/param.h>

/** @name Codes for general mutation types */
//@{
#define F4_ADD           0 ///<Adding new node
#define F4_DEL           1 ///<Deletion of node
#define F4_MOD           2 ///<Modification of node
#define F4_COUNT         3 ///<Count of mutation types
//@}

/** @name Codes for specific F4_ADD mutation subtypes */
//@{
#define F4_ADD_DIV       0
#define F4_ADD_CONN      1
#define F4_ADD_NEUPAR    2
#define F4_ADD_REP       3
#define F4_ADD_SIMP      4
#define F4_ADD_COUNT     5
//@}

class Geno_f4 : public GenoOperators
{
public:
	Geno_f4();
	void setDefaults();

	int checkValidity(const char *, const char *genoname);
	int validate(char *&, const char *genoname);
	int mutate(char *& g, float & chg, int &method);
	int crossOver(char *&g1, char *&g2, float& chg1, float& chg2);
	const char* getSimplest() { return "X"; }
	uint32_t style(const char *g, int pos);

	// mutation probabilities
	double prob[F4_COUNT];            ///<relative probabilities of selecting mutation types in f4 genotype
	double probadd[F4_ADD_COUNT];     ///<relative probabilities of selecting mutation addition subtypes

	SString excluded_modifiers;       ///<Modifiers that are excluded in mutation process
	static const char *all_modifiers;

protected:

	/**
	 * Validates a f4 genotype. If the genotype is invalid, the genotype is repaired
	 * and the validation is repeated. Validation is performed as long as repairing
	 * is not effective, or the number of retries exceeded the given limit.
	 * @param geno genotype tree
	 * @param retrycount maximum amount of repair retries
	 * @return GENOOPER_OK if genotype is valid, GENOPER_REPAIR if genotype can be repaired, GENOPER_OPFAIL if genotype can't be repaired
	 */
	int  ValidateRec(f4_node *geno, int retrycount) const;

	/**
	 * Performs mutation of an f4 genotype. The mutation is performed on a random node
	 * from a given tree. The method of mutation is chosen via the roulette selection,
	 * where probabilities of choosing each mutation type are given in the 'prob'
	 * array. Possible mutation types are:
	 *  - F4_ADD - adds new element to the genotype by:
	 *   - F4_ADD_DIV - replacing randomly selected node with division node '<', setting this node as a child and creating new stick or neuron sibling of the selected cell (the neuron-type sibling will be connected to a random existing neuron),
	 *   - F4_ADD_CONN - adding connection for an existing neuron,
	 *   - F4_ADD_NEUPAR - adding neuron property to the selected node, if it is a neuron node,
	 *   - F4_ADD_REP - adding a repetition node before a randomly selected node (the repetition node has 2 repetitions),
	 *   - F4_ADD_SIMP - adding a simple node before a selected node,
	 *  - F4_DEL - removes a randomly selected node (the node needs to have a parent and at least one child, otherwise returns GENOPER_OPFAIL),
	 *  - F4_MOD - modifies one of simple nodes by:
	 *   - '<' - swapping children in division
	 *   - '[' - modifying connection of a neuron
	 *   - '#' - incrementing or decrementing repetition count
	 *
	 * @param g input genotype; the result of mutation will be stored in this parameter
	 * @param method reference to the variable that will get the selected method of mutation
	 * @return GENOPER_OK if mutation was performed successfully, GENOPER_FAIL otherwise
	 */
	int  MutateOne(f4_node *& g, int &method) const;

	/**
	 * Creates a random connection to an existing neuron or creates an additional
	 * sensor for a neuron.
	 * @param nn neuron class node
	 * @param neuid id of a neuron
	 * @param neulist list of genotype neuron classes
	 */
	void linkNodeMakeRandom(f4_node *nn, int neuid, std::vector<NeuroClass*> neulist) const;

	/**
	 * Changes connection to an existing neuron or creates an additional
	 * sensor for neuron.
	 * @param nn neuron connection node
	 * @param neuid id of a neuron
	 * @param neulist list of genotype neuron classes
	 */
	void linkNodeChangeRandom(f4_node *nn, int neuid, std::vector<NeuroClass*> neulist) const;

	/**
	 * Introduces a random modification to the neuron node.
	 * @param nn neuron node
	 */
	void nparNodeMakeRandom(f4_node *nn) const;

	/**
	 * Increases or decreases the amount of repetitions in the repetition node.
	 * @param nn repetition node
	 */
	void repeatNodeChangeRandom(f4_node *nn) const;

	/**
	 * Tries to perform a mutation until success. There is a maximum of 20 tries. Returns GENOPER_OK or GENOPER_OPFAIL.
	 * @param g genotype tree
	 * @param method reference to the variable that will get the selected method of mutation
	 * @return GENOPER_OK if performed successful mutation, GENOPER_FAIL otherwise
	 */
	int  MutateOneValid(f4_node *&g, int &method) const;

	/**
	 * Performs crossover of two creatures. The 'chg' parameter determines approximately what
	 * percentage of the first creature should form the offspring. '1-chg' is the percentage
	 * of the second creature in the offspring.
	 * @param g1 first parent
	 * @param g2 second parent
	 * @param chg percentage of the first parent in offspring (the second parent has the rest)
	 */
	int  CrossOverOne(f4_node *g1, f4_node *g2, float chg) const;
};


#endif

