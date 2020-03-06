// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

// Copyright (C) 1999,2000  Adam Rotaru-Varga (adam_rotaru@yahoo.com), GNU LGPL
// Copyright (C) since 2001 Maciej Komosinski
// 2018, Grzegorz Latosinski, added support for new API for neuron types and their properties

#ifndef _F4_CONV_H_
#define _F4_CONV_H_

#include <frams/model/model.h>
#include <frams/model/modelparts.h>
#include <frams/genetics/genoconv.h>
#include "f4_general.h"


/**
 * Genotype converter from f4 to f0.
 */
class GenoConv_f40 : public GenoConverter
{
public:
	GenoConv_f40();

	/**
	 * Performs conversion from f4 to f0. Creates f4_Model from f4 genotype
	 * and converts the Model to the f0 genotype string.
	 * @param in f4 genotype
	 * @param map mapping from f4 to Model
	 * @param using_checkpoints determines if checkpoints will be available
	 * @return generated f0 genotype
	 */
	SString convert(SString &in, MultiMap *map, bool using_checkpoints);
};


/**
 * Genotype converter from f4 to f1. This is only experimental conversion and
 * returns an approximate f1 genotype.
 */
class GenoConv_F41_TestOnly : public GenoConverter
{
public:
	/**
	 * Initializes converter.
	 */
	GenoConv_F41_TestOnly();

	/**
	 * Performs conversion from f4 to f1. Creates f4_Model from f4 genotype
	 * and converts the Model to the f1 genotype string. The final f1 genotype is
	 * an approximation.
	 * @param in f4 genotype
	 * @param map mapping from f4 to Model
	 * @param using_checkpoints determines if checkpoints will be available
	 * @return generated approximate f1 genotype
	 */
	SString convert(SString &in, MultiMap *map, bool using_checkpoints);
};


/**
 * A Model descendant which supports building from an f4 genotype.
 */
class f4_Model : public Model
{
public:
	f4_Model();
	~f4_Model();

	/**
	 * Builds a Model from the f4 genotype string.
	 * @param geno genotype string
	 * @return GENOPER_OK if a Model could be created, error code otherwise
	 */
	int      buildFromF4(SString &geno, bool using_checkpoints);
	/**
	 * Outputs a Model in f1 format. It is an approximation of the input f4 genotype.
	 * @param out the reference that stores the conversion result
	 */
	void     toF1Geno(SString &out);       // output to f1 format, approximation
private:
	f4_Cells *cells;
	int        buildModelRec(f4_Cell *ndad);
	/**
	 * Get a cell which is a stick, by traversing dadlinks.
	 */
	f4_Cell* getStick(f4_Cell *C);
	int        error;
	int        errorpos;
};


#endif
