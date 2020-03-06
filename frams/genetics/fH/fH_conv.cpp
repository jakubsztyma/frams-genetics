// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "fH_conv.h"

SString GenoConv_fH0::convert(SString &i, MultiMap *map, bool using_checkpoints)
{
	fH_Builder builder(0, NULL != map);
	builder.parseGenotype(i);
	if (!builder.sticksExist()) return "";
	Model * m = builder.buildModel(using_checkpoints);
	if (!m) return SString(); // oops
	if (NULL != map)
		// generate to-f0 conversion map
		m->getCurrentToF0Map(*map);
	SString gen = m->getF0Geno().getGenes();
	delete m;
	return gen;
}
