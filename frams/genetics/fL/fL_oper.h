// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FL_OPER_H_
#define _FL_OPER_H_

#include "../genooperators.h"
#include "fL_general.h"

/** @name Codes for general fL mutation types */
//@{
#define FL_ADD_WORD 0 ///<probability of adding word to axiom or rule successor
#define FL_ADD_WDEF 1 ///<probability of adding word definition
#define FL_ADD_RULE 2 ///<probability of adding new rule
#define FL_CHG_COND 3 ///<probability of modification or rule condition
#define FL_CHG_WORD 4 ///<probability of changing word name or formula in axiom or rule successor
#define FL_CHG_ITER 5 ///<probability of changing iteration of genotype
#define FL_DEL_WORD 6 ///<probability of deleting word from axiom or rule. Deleting all occurrences of word deletes word definition. Deleting all words in rule deletes this rule
#define FL_OPCOUNT 7 ///<count of mutation operators
//@}

/** @name Codes for probabilities of mutating axiom words or rule words */
//@{
#define FL_AXM_WORD_MUT_PROB 0 ///<probability of performing addition, substitution or deletion of word in axiom
#define FL_RUL_WORD_MUT_PROB 1 ///<probability of performing addition, substitution or deletion of word in rule's successor
#define FL_MUTGROUPSCOUNT 2 ///<count of possible groups for mutations
//@}

/** @name Codes for probabilities of mutating word names or formula during change */
//@{
#define FL_CHG_WORD_FORMULA 0 ///<probability of changing formula of one of parameters
#define FL_CHG_WORD_NAME 1 ///<probability of changing word name to other word name with <= number of parameters
#define FL_CHG_COUNT 2 ///<count of possible changes in words
//@}

/** @name Codes for probabilities of choosing one of word types for addition */
//@{
#define FL_ADD_STICK  0 ///<probability of adding stick
#define FL_ADD_NEURO  1 ///<probability of adding neuron
#define FL_ADD_CONN   2 ///<probability of adding connection
#define FL_ADD_ROT    3 ///<probability of adding one of rotation words
#define FL_ADD_OTHER  4 ///<probability of adding word defined in the genotype
#define FL_ADD_BRANCH 5 ///<probability of adding a branch
#define FL_ADD_COUNT  6 ///<count of possible additions
//@}

class Geno_fL : public GenoOperators
{
private:

	/**
	 * Adds word with a given definition to the list in place pointed by an iterator.
	 * @param list list, to which new word will be added
	 * @param definition temporal object that will act as pattern for newly created word
	 * @param it the iterator pointing to addition point
	 * @return true
	 */
	bool addWord(std::list<fL_Word *>* list, fL_Word *definition, std::list<fL_Word *>::iterator it);

	/**
	 * Selects axiom or one of rule's successors.
	 * @param creature the object with defined axiom and rules
	 * @param numparams reference holding the number of parameters that are available for this list, 0 for axiom
	 * @param ruleid the index of the rule in rules structure or -1 if selected sequence is the axiom
	 * @return pointer to a selected sequence
	 */
	std::list<fL_Word *>* selectRandomSequence(fL_Builder *creature, int &numparams, int &ruleid);

	/**
	 * Selects word definition according to a method. Method is one of
	 * values FL_ADD_STICK, FL_ADD_NEURO, FL_ADD_CONN, FL_ADD_ROT, FL_ADD_OTHER
	 * etc. If FL_ADD_OTHER is chosen, then one of defined words is chosen. If
	 * there are no defined words, then one of built-in words is chosen.
	 * If FL_ADD_BRANCH is selected, then method returns NULL.
	 * @param creature current genotype
	 * @param method one of methods of addition
	 * @return object defining one of genotype words, or NULL if branching method is used
	 */
	fL_Word* randomWordDefinition(fL_Builder *creature, int method);

	/**
	 * Tries to find appropriate word in second creature that matches the word in first creature.
	 * Firstly, it tries to check if some word is not already assigned to a word in second creature.
	 * If the searching is successful, then appropriate word is used. Otherwise, method tries to find
	 * word that matches by name and number of parameters, or at least by number of parameters and hasn't
	 * been used already for other translation. If this is impossible, method creates new word definition
	 * for the second creature.
	 * @param from creature, from rule is taken
	 * @param to creature, which takes the rule
	 * @param fromword word from the first creature that needs to be translated
	 * @param map hashmap for current assignments
	 * @return word instance that need to be used by generated rule for the second genotype
	 */
	fL_Word* getAppropriateWord(fL_Builder *from, fL_Builder *to, fL_Word *fromword, std::unordered_map<std::string, std::string> &map);

	/**
	 * Migrates random rule from one creature to the other creature.
	 * @param from creature, from rule is taken
	 * @param to creature, which takes the rule
	 * @param numselrules number of rules that need to be moved
	 */
	void migrateRandomRules(fL_Builder *from, fL_Builder *to, int numselrules);

	/**
	 * Deletes branch from a given sequence starting from iterator. The deletion
	 * removes only braces, not whole branch.
	 * @param list current list that needs to be modified
	 * @param openbranchposition the iterator pointing to the open branch word
	 */
	void deleteBranch(std::list<fL_Word *> *list, std::list<fL_Word *>::iterator openbranchposition);
public:
	double operations[FL_OPCOUNT]; ///<Relative probabilities of mutation types
	double groupprobabilities[FL_MUTGROUPSCOUNT]; ///<Relative probabilities of changing elements in rules or axioms
	double chgoperations[FL_CHG_COUNT]; ///<Relative probabilities of changing word names or parameters of word during change mutation
	double addtypes[FL_ADD_COUNT]; ///<Relative probabilities of selecting special word types

	double iterchangestep; ///<minimal value, by which time of development is modified
	int maxdefinedwords; ///<maximal number of defined words for single fL genotype

	Geno_fL();

	int checkValidity(const char *geno, const char *genoname);

	int validate(char *&geno, const char *genoname);

	int mutate(char *&geno, float& chg, int &method);

	int crossOver(char *&g1, char *&g2, float& chg1, float& chg2);

	const char* getSimplest() { return "i:axiom=\"S()\", time=0, maxwords=300"; }

	uint32_t style(const char *geno, int pos);
};

#endif // _FL_OPER_H_
