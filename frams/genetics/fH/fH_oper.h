// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FH_OPER_H_
#define _FH_OPER_H_

#include "../genooperators.h"
#include "fH_general.h"

/** @name Codes for general fH mutation types */
//@{
#define FH_ADD     0 ///<Adding new handle
#define FH_HANDLE  1 ///<Modifying vectors of handles
#define FH_PROP    2 ///<Modifying properties of sticks/neurons/connections
#define FH_DEL     3 ///<Deleting single handle from genotype. WARNING! this type of mutation must be last in defines, because later during mutation this method may be skipped due to the lack of genotypes to remove
#define FH_OPCOUNT 4 ///<Count of mutation types
//@}

/** @name Codes for specific FH_ADD mutation subtypes */
//@{
#define FH_ADD_STICK 0 ///<Adding stick
#define FH_ADD_NEURO 1 ///<Adding neuron
#define FH_ADD_CONN  2 ///<Adding connection
#define FH_ADD_OPCOUNT 3 ///<Count of addition types
//@}

class Geno_fH : public GenoOperators
{
private:
	/**
	 * Mutates vectors and/or properties of a given handle.
	 * Properties and vector values are set with mutateCreep method, with respect to their
	 * minimal, maximal and default values. Only weights of connections are mutated
	 * with mutateNeuProperty.
	 * @param handle handle that has to be modified
	 * @param tab ParamTab holding definitions of properties of a given handle
	 * @param dimensions number of values stored in each vector of handle
	 * @param changedimensions true if values of vectors should be changed, false otherwise
	 * @param changeproperties true if values of properties should be changed, false otherwise
	 */
	void mutateHandleValues(fH_Handle *handle, ParamEntry *tab, int dimensions, bool changedimensions, bool changeproperties);

	/**
	 * Creates new handle with random vectors and one mutated property. Vector values are
	 * set to values from [-1,1] - probabilities of each values are evenly distributed.
	 * It also creates required obj for handle and initializes it with default
	 * values.
	 * @param handle the handle for which obj is created and mutation is performed
	 * @param tab the corresponding ParamTab for handle
	 * @param dimensions number of values in vectors
	 */
	void createHandleVectors(fH_Handle *handle, ParamEntry *tab, int dimensions);

	/**
	 * Mutates "details" property of neurons. First it tries to parse neuron class
	 * given in "details" field. It it fails, or "userandomclass" parameter is set
	 * to true, then new available class is picked (or "N", if there are no
	 * available classes). When neuron class is established, method iterates through
	 * neuron properties and mutates them with mutateNeuProperty method.
	 * @param handle handle that has to be modified
	 * @param tab ParamTab holding definitions of properties of a given handle
	 * @param userandomclass true if method should find random class, false if current class or "N" should be used
	 */
	void mutateNeuronHandleProperties(fH_NeuronHandle *handle, ParamEntry *tab, bool userandomclass = false);

	/**
	 * Mutates single double property in par, accessed by id. If property is weight
	 * of connection, then it is mutated with mutateNeuProperty.
	 * @param id numerical id of property in Param
	 * @param par Param with properties to modify
	 * @param type used to determine if property is weight of connection
	 */
	void changeDoubleProperty(int id, Param &par, fHBodyType type);

	/**
	 * Gets random handle and corresponding ParamTab from a given body. Both pointers
	 * are returned in parameters "handle" and "tab" respectively. Parameter
	 * "skipalonestick" should be set if method selects handle for deleting (valid
	 * body should have at least one stick).
	 * @param creature Builder object with parsed genotype, from which random handle is selected
	 * @param handle reference to pointer pointing to selected handle
	 * @param tab reference to pointer pointing to corresponding ParamTab
	 * @param skipalonestick true if method should skip only stick during random selection of handles, false if method should randomly choose handle from all available handles
	 * @return position of handle pointer in vector of handles of same type in Builder (if selected handle is of STICK type, then returned value is its position in "sticks" vector of Builder)
	 */
	unsigned int getRandomHandle(fH_Builder *creature, fH_Handle *&handle, ParamEntry *&tab, bool skipalonestick = false);

public:
	double operations[FH_OPCOUNT];    ///<relative probabilities of selecting mutation types in fH genotype
	double addoperations[FH_ADD_OPCOUNT]; ///<relative probabilities of selecting mutation addition subtypes

	// TODO add to GenoOperators?
	/**
	 * Mutates properties of neuron and returns full neuron class description.
	 * @param det current neuron class definition with its parameters
	 */
	static void mutateNeuronProperties(SString &det);

	Geno_fH();

	int checkValidity(const char *geno, const char *genoname);

	int validate(char *&geno, const char *genoname);

	int mutate(char *&geno, float& chg, int &method);

	int crossOver(char *&g1, char *&g2, float& chg1, float& chg2);

	virtual const char* getSimplest() { return "3\nj:"; }

	uint32_t style(const char *geno, int pos);
};

#endif //_FH_OPER_H_
