// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "fL_conv.h"
#include "fL_general.h"

SString GenoConv_fL0::convert(SString &i, MultiMap *map, bool using_checkpoints)
{
	fL_Builder builder(NULL != map, using_checkpoints);
	if (builder.parseGenotype(i) != 0)  return "";
	double neededtime;
	Model *m = builder.developModel(neededtime);
	if (!m) return SString();
	if (NULL != map)
		m->getCurrentToF0Map(*map);
	SString gen = m->getF0Geno().getGenes();
	delete m;
	return gen;
}
