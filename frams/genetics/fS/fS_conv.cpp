// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 2019-2020  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "fS_conv.h"

SString GenoConv_fS0::convert(SString &i, MultiMap *map, bool using_checkpoints)
{
	fS_Genotype *genotype;
	try
	{
		genotype = new fS_Genotype(i.c_str());
	}
	catch (fS_Exception &e)
	{
		logPrintf("GenoConv_fS0", "convert", LOG_ERROR, e.what());
		return SString();
	}

	Model model = genotype->buildModel(using_checkpoints);
	delete genotype;

	if (map)
	{
		model.getCurrentToF0Map(*map);
	}

	return model.getF0Geno().getGenes();
}
